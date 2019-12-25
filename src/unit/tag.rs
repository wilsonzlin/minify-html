use crate::err::{ErrorType, InternalResult};
use crate::proc::Processor;
use crate::spec::codepoint::{is_alphanumeric, is_whitespace};
use crate::spec::tag::void::VOID_TAGS;
use crate::unit::attr::{AttrType, process_attr};
use crate::unit::content::process_content;
use crate::unit::script::process_script;
use crate::unit::style::process_style;
use std::io::{stdout, Write};

// Tag names may only use ASCII alphanumerics. However, some people also use `:` and `-`.
// See https://html.spec.whatwg.org/multipage/syntax.html#syntax-tag-name for spec.
fn is_valid_tag_name_char(c: u8) -> bool {
    is_alphanumeric(c) || c == b':' || c == b'-'
}

pub fn process_tag<'d, 'p>(proc: &'p mut Processor<'d>) -> InternalResult<()> {
    // TODO Minify opening and closing tag whitespace before name and after name/last attr.
    // TODO DOC No checking if opening and closing names match.
    // Expect to be currently at an opening tag.
    chain!(proc.match_char(b'<').expect().keep());
    // May not be valid tag name at current position, so require instead of expect.
    let opening_name_range = chain!(proc.match_while_pred(is_valid_tag_name_char).require_with_reason("tag name")?.keep().range());

    let mut last_attr_type = AttrType::None;
    let mut self_closing = false;

    loop {
        // At the beginning of this loop, the last parsed unit was
        // either the tag name or an attribute (including its value, if
        // it had one).
        let ws_accepted = chain!(proc.match_while_pred(is_whitespace).discard().matched());

        if chain!(proc.match_char(b'>').keep().matched()) {
            // End of tag.
            break;
        }

        self_closing = chain!(proc.match_seq(b"/>").keep().matched());
        if self_closing {
            break;
        }

        // HB_ERR_PARSE_NO_SPACE_BEFORE_ATTR is not suppressible as
        // otherwise there would be difficulty in determining what is
        // the end of a tag/attribute name/attribute value.
        if !ws_accepted {
            return Err(ErrorType::NoSpaceBeforeAttr);
        }

        if last_attr_type != AttrType::Quoted {
            proc.write(b' ');
        }

        last_attr_type = process_attr(proc)?;
    };

    if self_closing || VOID_TAGS.contains(&proc[opening_name_range]) {
        return Ok(());
    };

    // TODO DOC: Tags must be case sensitive.
    match &proc[opening_name_range] {
        b"script" => process_script(proc)?,
        b"style" => process_style(proc)?,
        _ => process_content(proc, Some(opening_name_range))?,
    };

    // Require closing tag for non-void.
    chain!(proc.match_seq(b"</").require_with_reason("closing tag")?.keep());
    chain!(proc.match_while_pred(is_valid_tag_name_char).require_with_reason("closing tag name")?.keep());
    chain!(proc.match_char(b'>').require_with_reason("closing tag")?.keep());
    Ok(())
}
