use crate::err::{ErrorType, ProcessingResult};
use crate::proc::Processor;

fn is_string_delimiter(c: u8) -> bool {
    match c {
        b'"' | b'\'' => true,
        _ => false,
    }
}

fn parse_comment(proc: &mut Processor) -> ProcessingResult<()> {
    if cfg!(debug_assertions) {
        chain!(proc.match_seq(b"/*").expect().keep());
    } else {
        proc.skip_amount_expect(2);
    };

    // Unlike script tags, style comments do NOT end at closing tag.
    while !chain!(proc.match_seq(b"*/").keep().matched()) {
        proc.accept()?;
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
    while !chain!(proc.match_seq(b"</").matched()) {
        if chain!(proc.match_seq(b"/*").matched()) {
            parse_comment(proc)?;
        } else if chain!(proc.match_pred(is_string_delimiter).matched()) {
            parse_string(proc)?;
        } else {
            proc.accept()?;
        }
    };

    Ok(())
}
