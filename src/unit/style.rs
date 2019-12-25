use crate::proc::Processor;
use crate::err::{HbRes, HbErr};

fn is_string_delimiter(c: u8) -> bool {
    match c {
        b'"' | b'\'' => true,
        _ => false,
    }
}

fn parse_comment<'d, 'p>(proc: &'p mut Processor<'d>) -> HbRes<()> {
    cascade_return!(proc.match_seq(b"/*").expect().keep());

    // Unlike script tags, style comments do NOT end at closing tag.
    while !cascade_return!(proc.match_seq(b"*/").keep().matched()) {
        proc.accept();
    };

    Ok(())
}

fn parse_string<'d, 'p>(proc: &'p mut Processor<'d>) -> HbRes<()> {
    let delim = cascade_return!(proc.match_pred(is_string_delimiter).expect().keep().char());

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

        if cascade_return!(proc.match_line_terminator().keep().matched()) {
            if !escaping {
                // TODO Use better error type.
                return Err(HbErr::ExpectedNotFound("Unterminated CSS string"));
            }
        }

        escaping = false;
    };

    Ok(())
}

pub fn process_style<'d, 'p>(proc: &'p mut Processor<'d>) -> HbRes<()> {
    while !cascade_return!(proc.match_seq(b"</").matched()) {
        if cascade_return!(proc.match_seq(b"/*").matched()) {
            parse_comment(proc)?;
        } else if cascade_return!(proc.match_pred(is_string_delimiter).matched()) {
            parse_string(proc)?;
        } else {
            proc.accept()?;
        }
    };

    Ok(())
}
