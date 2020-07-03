use lazy_static::lazy_static;
use std::collections::HashSet;
use crate::err::{ErrorType, ProcessingResult};
use crate::proc::checkpoint::Checkpoint;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
use crate::proc::range::ProcessorRange;
use crate::spec::codepoint::{is_alphanumeric, is_whitespace};
use crate::spec::tag::omission::CLOSING_TAG_OMISSION_RULES;
use crate::spec::tag::void::VOID_TAGS;
use crate::unit::attr::{AttrType, process_attr, ProcessedAttr};
use crate::unit::content::process_content;
use crate::unit::script::process_script;
use crate::unit::style::process_style;
use crate::gen::attrs::{ATTRS, AttributeMinification};
use crate::spec::tag::ns::Namespace;

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

// Tag names may only use ASCII alphanumerics. However, some people also use `:` and `-`.
// See https://html.spec.whatwg.org/multipage/syntax.html#syntax-tag-name for spec.
fn is_valid_tag_name_char(c: u8) -> bool {
    is_alphanumeric(c) || c == b':' || c == b'-'
}

#[derive(Copy, Clone)]
enum TagType {
    Script,
    Style,
    Other,
}

#[derive(Copy, Clone)]
pub struct MaybeClosingTag(Option<ProcessorRange>);

impl MaybeClosingTag {
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
pub fn process_tag(proc: &mut Processor, ns: Namespace, mut prev_sibling_closing_tag: MaybeClosingTag) -> ProcessingResult<MaybeClosingTag> {
    // Expect to be currently at an opening tag.
    proc.m(IsChar(b'<'), Discard).expect();
    // May not be valid tag name at current position, so require instead of expect.
    let source_tag_name = proc.m(WhilePred(is_valid_tag_name_char), Discard).require("tag name")?;
    if prev_sibling_closing_tag.exists_and(|prev_tag|
        CLOSING_TAG_OMISSION_RULES
            .get(&proc[prev_tag])
            .filter(|rule| rule.can_omit_as_before(&proc[source_tag_name]))
            .is_none()
    ) {
        prev_sibling_closing_tag.write(proc);
    };
    // Write initially skipped left chevron.
    proc.write(b'<');
    // Write previously skipped name and use written code as range (otherwise source code will eventually be overwritten).
    let tag_name = proc.write_range(source_tag_name);

    let tag_type = match &proc[tag_name] {
        b"script" => TagType::Script,
        b"style" => TagType::Style,
        _ => TagType::Other,
    };

    let mut last_attr_type: Option<AttrType> = None;
    let mut self_closing = false;
    let is_void_tag = VOID_TAGS.contains(&proc[tag_name]);

    loop {
        // At the beginning of this loop, the last parsed unit was either the tag name or an attribute (including its value, if it had one).
        proc.m(WhilePred(is_whitespace), Discard);

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
        let attr_checkpoint = Checkpoint::new(proc);
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
            (TagType::Script, b"type") => {
                // It's JS if the value is empty or one of `JAVASCRIPT_MIME_TYPES`.
                let script_tag_type_is_js = value
                    .filter(|v| !JAVASCRIPT_MIME_TYPES.contains(&proc[*v]))
                    .is_none();
                if script_tag_type_is_js {
                    erase_attr = true;
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

    match tag_type {
        TagType::Script => process_script(proc)?,
        TagType::Style => process_style(proc)?,
        _ => process_content(proc, child_ns, Some(tag_name))?,
    };

    // Require closing tag for non-void.
    proc.m(IsSeq(b"</"), Discard).require("closing tag")?;
    let closing_tag = proc.m(WhilePred(is_valid_tag_name_char), Discard).require("closing tag name")?;
    // We need to check closing tag matches as otherwise when we later write closing tag, it might be longer than source closing tag and cause source to be overwritten.
    if !proc[closing_tag].eq(&proc[tag_name]) {
        return Err(ErrorType::ClosingTagMismatch);
    };
    proc.m(WhilePred(is_whitespace), Discard);
    proc.m(IsChar(b'>'), Discard).require("closing tag end")?;
    Ok(MaybeClosingTag(Some(tag_name)))
}
