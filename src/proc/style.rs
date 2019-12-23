use crate::proc::Processor;
use crate::err::{HbRes, HbErr};
use crate::code::Code;

fn is_string_delimiter(c: u8) -> bool {
    match c {
        b'"' | b'\'' => true,
        _ => false,
    }
}

fn parse_comment<D: Code>(proc: &Processor<D>) -> HbRes<()> {
    proc.match_seq(b"/*").expect().keep();

    // Unlike script tags, style comments do NOT end at closing tag.
    while !proc.match_seq(b"*/").keep().matched() {
        proc.accept();
    };

    Ok(())
}

fn parse_string<D: Code>(proc: &Processor<D>) -> HbRes<()> {
    let delim = proc.match_pred(is_string_delimiter).expect().keep().char();

    let mut escaping = false;

    loop {
        let c = proc.accept()?;

        if c == b'\\' {
            escaping = !escaping;
            continue;
        }

        if c == delim && !escaping {
            break;
        }

        if proc.match_line_terminator().keep().matched() {
            if !escaping {
                // TODO Use better error type.
                return Err(HbErr::ExpectedNotFound("Unterminated CSS string"));
            }
        }

        escaping = false;
    };

    Ok(())
}

pub fn process_style<D: Code>(proc: &Processor<D>) -> HbRes<()> {
    while !proc.match_seq(b"</").matched() {
        if proc.match_seq(b"/*").matched() {
            parse_comment(proc)?;
        } else if proc.match_pred(is_string_delimiter).matched() {
            parse_string(proc)?;
        } else {
            proc.accept()?;
        }
    };

    Ok(())
}
