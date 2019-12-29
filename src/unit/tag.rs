use crate::err::{ErrorType, ProcessingResult};
use crate::proc::{Processor, ProcessorRange};
use crate::spec::codepoint::{is_alphanumeric, is_whitespace};
use crate::spec::tag::void::VOID_TAGS;
use crate::unit::attr::{AttrType, process_attr, ProcessedAttr};
use crate::unit::content::process_content;
use crate::unit::style::process_style;
use phf::{Set, phf_set};
use crate::unit::script::text::process_text_script;
use crate::unit::script::js::process_js_script;

pub static JAVASCRIPT_MIME_TYPES: Set<&'static [u8]> = phf_set! {
    b"application/ecmascript",
    b"application/javascript",
    b"application/x-ecmascript",
    b"application/x-javascript",
    b"text/ecmascript",
    b"text/javascript",
    b"text/javascript1.0",
    b"text/javascript1.1",
    b"text/javascript1.2",
    b"text/javascript1.3",
    b"text/javascript1.4",
    b"text/javascript1.5",
    b"text/jscript",
    b"text/livescript",
    b"text/x-ecmascript",
    b"text/x-javascript",
};

// Tag names may only use ASCII alphanumerics. However, some people also use `:` and `-`.
// See https://html.spec.whatwg.org/multipage/syntax.html#syntax-tag-name for spec.
fn is_valid_tag_name_char(c: u8) -> bool {
    is_alphanumeric(c) || c == b':' || c == b'-'
}

enum TagType {
    Script,
    Style,
    Other,
}

pub fn process_tag(proc: &mut Processor) -> ProcessingResult<()> {
    // TODO Minify opening and closing tag whitespace before name and after name/last attr.
    // TODO DOC No checking if opening and closing names match.
    // Expect to be currently at an opening tag.
    chain!(proc.match_char(b'<').expect().keep());
    // May not be valid tag name at current position, so require instead of expect.
    let opening_name_range = chain!(proc.match_while_pred(is_valid_tag_name_char).require_with_reason("tag name")?.keep().out_range());

    // TODO DOC: Tags must be case sensitive.
    let tag_type = match &proc[opening_name_range] {
        b"script" => TagType::Script,
        b"style" => TagType::Style,
        _ => TagType::Other,
    };

    let mut last_attr_type: Option<AttrType> = None;
    let mut self_closing = false;
    // Value of any "type" attribute; if multiple, last kept.
    let mut attr_type_value: Option<ProcessorRange> = None;

    loop {
        // At the beginning of this loop, the last parsed unit was either the tag name or an attribute (including its value, if it had one).
        let ws_accepted = chain!(proc.match_while_pred(is_whitespace).discard().matched());

        if chain!(proc.match_char(b'>').keep().matched()) {
            // End of tag.
            break;
        }

        self_closing = chain!(proc.match_seq(b"/>").keep().matched());
        if self_closing {
            break;
        }

        // This needs to be enforced as otherwise there would be difficulty in determining what is the end of a tag/attribute name/attribute value.
        if !ws_accepted {
            return Err(ErrorType::NoSpaceBeforeAttr);
        }

        // Write space after tag name or unquoted/valueless attribute.
        // Don't write after unquoted.
        match last_attr_type {
            Some(AttrType::Unquoted) | Some(AttrType::NoValue) | None => proc.write(b' '),
            _ => {}
        };

        let ProcessedAttr { name, typ, value } = process_attr(proc)?;
        match &proc[name] {
            b"type" => attr_type_value = value,
            _ => {},
        };
        last_attr_type = Some(typ);
    };

    if self_closing || VOID_TAGS.contains(&proc[opening_name_range]) {
        return Ok(());
    };

    match tag_type {
        TagType::Script => if attr_type_value.is_none() || attr_type_value.filter(|n| JAVASCRIPT_MIME_TYPES.contains(&proc[*n])).is_some() {
            process_js_script(proc)?;
        } else {
            process_text_script(proc)?;
        },
        TagType::Style => process_style(proc)?,
        _ => process_content(proc, Some(opening_name_range))?,
    };

    // Require closing tag for non-void.
    chain!(proc.match_seq(b"</").require()?.keep());
    chain!(proc.match_while_pred(is_valid_tag_name_char).require_with_reason("closing tag name")?.keep());
    chain!(proc.match_char(b'>').require()?.keep());
    Ok(())
}
