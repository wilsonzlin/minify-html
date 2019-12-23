use crate::proc::attr::{AttrType, process_attr};
use crate::err::{HbRes, HbErr};
use crate::proc::Processor;
use crate::spec::codepoint::{is_alphanumeric, is_whitespace};
use crate::proc::content::process_content;
use crate::proc::script::process_script;
use crate::proc::style::process_style;
use crate::spec::tag::void::VOID_TAGS;
use crate::code::Code;

// Tag names may only use ASCII alphanumerics. However, some people also use `:` and `-`.
// See https://html.spec.whatwg.org/multipage/syntax.html#syntax-tag-name for spec.
fn is_valid_tag_name_char(c: u8) -> bool {
    is_alphanumeric(c) || c == b':' || c == b'-'
}

fn process_tag_name<'d, D: Code>(proc: &Processor<'d, D>) -> HbRes<&'d [u8]> {
    Ok(proc.while_pred(is_valid_tag_name_char).require_reason("tag name")?.accept().slice())
}

pub fn process_tag<D: Code>(proc: &Processor<D>, parent: Option<&[u8]>) -> HbRes<()> {
    proc.is('<').require().accept();
    let name = process_tag_name(proc)?;

    let mut last_attr_type = AttrType::None;
    let mut self_closing = false;

    loop {
        // At the beginning of this loop, the last parsed unit was
        // either the tag name or an attribute (including its value, if
        // it had one).
        let ws_accepted = proc.match_while_pred(is_whitespace).discard().count();

        if proc.match_char(b'>').keep().matched() {
            // End of tag.
            break;
        }

        if self_closing = proc.match_seq(b"/>").keep().matched() {
            break;
        }

        // HB_ERR_PARSE_NO_SPACE_BEFORE_ATTR is not suppressible as
        // otherwise there would be difficulty in determining what is
        // the end of a tag/attribute name/attribute value.
        if !ws_accepted {
            return Err(HbErr::NoSpaceBeforeAttr);
        }

        if last_attr_type != AttrType::Quoted {
            proc.write(b' ');
        }

        last_attr_type = process_attr(proc)?;
    }

    if self_closing || VOID_TAGS.contains(&name) {
        return Ok(());
    }

    // TODO WARNING: Tags must be case sensitive.
    match name {
        b"script" => process_script(proc)?,
        b"style" => process_style(proc)?,
        _ => process_content(proc, Some(name))?,
    }

    // Require closing tag for non-void.
    proc.match_seq(b"</").require_with_reason("closing tag")?.keep();
    let closing_name = process_tag_name(proc)?;
    if name != closing_name {
        // TODO Find a way to cleanly provide opening and closing tag
        // names (which are views) into error message without leaking
        // memory.
        return Err(HbErr::UnclosedTag);
    }
    proc.match_char(b'>').require_with_reason("closing tag")?.keep();
    Ok(())
}
