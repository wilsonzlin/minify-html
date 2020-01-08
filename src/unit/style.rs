use crate::err::{ErrorType, ProcessingResult};
use crate::proc::Processor;
use crate::spec::codepoint::is_whitespace;

fn is_string_delimiter(c: u8) -> bool {
    match c {
        b'"' | b'\'' => true,
        _ => false,
    }
}

fn parse_comment(proc: &mut Processor) -> ProcessingResult<()> {
    if cfg!(debug_assertions) {
        chain!(proc.match_seq(b"/*").expect().discard());
    } else {
        proc.skip_amount_expect(2);
    };

    // Unlike script tags, style comments do NOT end at closing tag.
    while !chain!(proc.match_seq(b"*/").discard().matched()) {
        proc.skip()?;
    };

    Ok(())
}

fn parse_string(proc: &mut Processor) -> ProcessingResult<()> {
    let delim = if cfg!(debug_assertions) {
        chain!(proc.match_pred(is_string_delimiter).expect().keep().char())
    } else {
        proc.accept_expect()
    };

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

        if chain!(proc.match_line_terminator().keep().matched()) {
            if !escaping {
                return Err(ErrorType::UnterminatedCssString);
            }
        }

        escaping = false;
    };

    Ok(())
}

pub fn process_style(proc: &mut Processor) -> ProcessingResult<()> {
    // TODO Refactor
    chain!(proc.match_while_pred(is_whitespace).discard());
    // This variable is used so that trailing whitespace is simply trimmed/removed instead of collapsed.
    let mut discarded_whitespace = false;
    while !chain!(proc.match_seq(b"</").matched()) {
        if discarded_whitespace {
            proc.write(b' ');
            discarded_whitespace = false;
        };
        if chain!(proc.match_while_pred(is_whitespace).discard().matched()) {
            discarded_whitespace = true;
        } else if chain!(proc.match_seq(b"/*").matched()) {
            parse_comment(proc)?;
        } else if chain!(proc.match_pred(is_string_delimiter).matched()) {
            parse_string(proc)?;
        } else {
            proc.accept()?;
        };
    };

    Ok(())
}
