use lazy_static::lazy_static;
use std::collections::HashSet;
use crate::err::{ErrorType, ProcessingResult};
use crate::proc::checkpoint::{WriteCheckpoint, ReadCheckpoint};
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
use crate::proc::range::ProcessorRange;
use crate::spec::tag::void::VOID_TAGS;
use crate::unit::attr::{AttrType, process_attr, ProcessedAttr};
use crate::unit::content::process_content;
use crate::unit::script::process_script;
use crate::unit::style::process_style;
use crate::gen::attrs::{ATTRS, AttributeMinification};
use crate::spec::tag::ns::Namespace;
use crate::gen::codepoints::{TAG_NAME_CHAR, WHITESPACE};
use crate::cfg::Cfg;
use crate::spec::tag::omission::{can_omit_as_last_node, can_omit_as_before};

lazy_static! {
    pub static ref JAVASCRIPT_MIME_TYPES: HashSet<&'static [u8]> = {
        let mut s = HashSet::<&'static [u8]>::new();
        s.insert(b"application/ecmascript");
        s.insert(b"application/javascript");
        s.insert(b"application/x-ecmascript");
        s.insert(b"application/x-javascript");
        s.insert(b"text/ecmascript");
        s.insert(b"text/javascript");
        s.insert(b"text/javascript1.0");
        s.insert(b"text/javascript1.1");
        s.insert(b"text/javascript1.2");
        s.insert(b"text/javascript1.3");
        s.insert(b"text/javascript1.4");
        s.insert(b"text/javascript1.5");
        s.insert(b"text/jscript");
        s.insert(b"text/livescript");
        s.insert(b"text/x-ecmascript");
        s.insert(b"text/x-javascript");
        s
    };
}

#[derive(Copy, Clone)]
enum TagType {
    ScriptJs,
    ScriptData,
    Style,
    Other,
}

#[derive(Copy, Clone)]
pub struct MaybeClosingTag(Option<ProcessorRange>);

impl MaybeClosingTag {
    #[inline(always)]
    pub fn none() -> MaybeClosingTag {
        MaybeClosingTag(None)
    }

    #[inline(always)]
    pub fn write(&mut self, proc: &mut Processor) -> () {
        proc.write_slice(b"</");
        proc.write_range(self.0.take().unwrap());
        proc.write(b'>');
    }

    #[inline(always)]
    pub fn write_if_exists(&mut self, proc: &mut Processor) -> bool {
        self.0.take().filter(|tag| {
            proc.write_slice(b"</");
            proc.write_range(*tag);
            proc.write(b'>');
            true
        }).is_some()
    }

    #[inline(always)]
    pub fn exists(&self) -> bool {
        self.0.is_some()
    }

    #[inline(always)]
    pub fn exists_and<F: FnOnce(ProcessorRange) -> bool>(&self, pred: F) -> bool {
        match self.0 {
            Some(range) => pred(range),
            None => false,
        }
    }

    #[inline(always)]
    pub fn replace(&mut self, tag: MaybeClosingTag) -> () {
        self.0 = tag.0;
    }
}

// TODO Comment param `prev_sibling_closing_tag`.
pub fn process_tag(
    proc: &mut Processor,
    cfg: &Cfg,
    ns: Namespace,
    parent: Option<ProcessorRange>,
    descendant_of_pre: bool,
    mut prev_sibling_closing_tag: MaybeClosingTag,
    source_tag_name: ProcessorRange,
) -> ProcessingResult<MaybeClosingTag> {
    if prev_sibling_closing_tag.exists_and(|prev_tag| !can_omit_as_before(proc, Some(prev_tag), source_tag_name)) {
        prev_sibling_closing_tag.write(proc);
    };
    // Write initially skipped left chevron.
    proc.write(b'<');
    // Write previously skipped name and use written code as range (otherwise source code will eventually be overwritten).
    let tag_name = proc.write_range(source_tag_name);

    let mut tag_type = match &proc[tag_name] {
        // Unless non-JS MIME `type` is provided, `script` tags contain JS.
        b"script" => TagType::ScriptJs,
        b"style" => TagType::Style,
        _ => TagType::Other,
    };

    let mut last_attr_type: Option<AttrType> = None;
    let mut self_closing = false;
    let is_void_tag = VOID_TAGS.contains(&proc[tag_name]);

    loop {
        // At the beginning of this loop, the last parsed unit was either the tag name or an attribute (including its value, if it had one).
        proc.m(WhileInLookup(WHITESPACE), Discard);

        if proc.m(IsChar(b'>'), Keep).nonempty() {
            // End of tag.
            break;
        }

        // Don't write self closing "/>" as it could be shortened to ">" if void tag.
        self_closing = proc.m(IsSeq(b"/>"), Discard).nonempty();
        if self_closing {
            break;
        }

        // Mark attribute start in case we want to erase it completely.
        let attr_checkpoint = WriteCheckpoint::new(proc);
        let mut erase_attr = false;

        // Write space after tag name or unquoted/valueless attribute.
        // Don't write after quoted.
        // Handle rare case where file ends in opening tag before an attribute and no minification has been done yet,
        // e.g. `<-` (yes, that's the entire file).
        if proc.at_end() {
            return Err(ErrorType::UnexpectedEnd);
        };
        match last_attr_type {
            Some(AttrType::Unquoted) | Some(AttrType::NoValue) | None => proc.write(b' '),
            _ => {}
        };

        let ProcessedAttr { name, typ, value } = process_attr(proc, ns, tag_name)?;
        match (tag_type, &proc[name]) {
            // NOTE: We don't support multiple `type` attributes, so can't go from ScriptData => ScriptJs.
            (TagType::ScriptJs, b"type") => {
                // It's JS if the value is empty or one of `JAVASCRIPT_MIME_TYPES`.
                let script_tag_type_is_js = value
                    .filter(|v| !JAVASCRIPT_MIME_TYPES.contains(&proc[*v]))
                    .is_none();
                if script_tag_type_is_js {
                    erase_attr = true;
                } else {
                    // Tag does not contain JS, don't minify JS.
                    tag_type = TagType::ScriptData;
                };
            }
            (_, name) => {
                // TODO Check if HTML tag before checking if attribute removal applies to all elements.
                erase_attr = match (value, ATTRS.get(ns, &proc[tag_name], name)) {
                    (None, Some(AttributeMinification { redundant_if_empty: true, .. })) => true,
                    (Some(val), Some(AttributeMinification { default_value: Some(defval), .. })) => proc[val].eq(*defval),
                    _ => false,
                };
            }
        };
        if erase_attr {
            attr_checkpoint.erase_written(proc);
        } else {
            last_attr_type = Some(typ);
        };
    };

    // TODO Self closing does not actually close for HTML elements, but might close for foreign elements.
    // See spec for more details.
    if self_closing || is_void_tag {
        if self_closing {
            // Write discarded tag closing characters.
            if is_void_tag {
                proc.write_slice(b">");
            } else {
                if let Some(AttrType::Unquoted) = last_attr_type {
                    // Prevent `/` from being part of the value.
                    proc.write(b' ');
                };
                proc.write_slice(b"/>");
            };
        };
        return Ok(MaybeClosingTag(None));
    };

    let child_ns = if proc[tag_name].eq(b"svg") {
        Namespace::Svg
    } else {
        ns
    };

    let mut closing_tag_omitted = false;
    match tag_type {
        TagType::ScriptData => process_script(proc, cfg, false)?,
        TagType::ScriptJs => process_script(proc, cfg, true)?,
        TagType::Style => process_style(proc, cfg)?,
        _ => closing_tag_omitted = process_content(proc, cfg, child_ns, Some(tag_name), descendant_of_pre)?.closing_tag_omitted,
    };

    let can_omit_closing_tag = can_omit_as_last_node(proc, parent, tag_name);
    if closing_tag_omitted || proc.at_end() && can_omit_closing_tag {
        return Ok(MaybeClosingTag(None));
    };

    let closing_tag_checkpoint = ReadCheckpoint::new(proc);
    proc.m(IsSeq(b"</"), Discard).require("closing tag")?;
    let closing_tag = proc.m(WhileInLookup(TAG_NAME_CHAR), Discard).require("closing tag name")?;
    proc.make_lowercase(closing_tag);

    // We need to check closing tag matches as otherwise when we later write closing tag, it might be longer than source closing tag and cause source to be overwritten.
    if proc[closing_tag] != proc[tag_name] {
        if can_omit_closing_tag {
            closing_tag_checkpoint.restore(proc);
            Ok(MaybeClosingTag(None))
        } else {
            Err(ErrorType::ClosingTagMismatch {
                expected: unsafe { String::from_utf8_unchecked(proc[tag_name].to_vec()) },
                got: unsafe { String::from_utf8_unchecked(proc[closing_tag].to_vec()) },
            })
        }
    } else {
        proc.m(WhileInLookup(WHITESPACE), Discard);
        proc.m(IsChar(b'>'), Discard).require("closing tag end")?;
        Ok(MaybeClosingTag(Some(tag_name)))
    }
}
