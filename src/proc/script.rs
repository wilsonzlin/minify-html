use crate::err::{HbRes, HbErr};
use crate::proc::{Processor};
use crate::code::Code;

fn is_string_delimiter(c: u8) -> bool {
    c == b'"' || c == b'\''
}

fn parse_comment_single<D: Code>(proc: &Processor<D>) -> HbRes<()> {
    proc.match_seq(b"//").expect().keep();

    // Comment can end at closing </script>.
    // WARNING: Closing tag must not contain whitespace.
    // TODO Optimise
    while !proc.match_line_terminator().keep().matched() {
        if proc.match_seq_i(b"</script>").matched() {
            break;
        }

        proc.accept()?;
    }

    Ok(())
}

fn parse_comment_multi<D: Code>(proc: &Processor<D>) -> HbRes<()> {
    proc.match_seq(b"/*").expect().keep();

    // Comment can end at closing </script>.
    // WARNING: Closing tag must not contain whitespace.
    // TODO Optimise
    while !proc.match_seq(b"*/").keep().matched() {
        if proc.match_seq_i(b"</script>").matched() {
            break;
        }

        proc.accept()?;
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
                return Err(HbErr::ExpectedNotFound("Unterminated JavaScript string"));
            }
        }

        escaping = false;
    };

    Ok(())
}

fn parse_template<D: Code>(proc: &Processor<D>) -> HbRes<()> {
    proc.match_char(b'`').expect().keep();

    let mut escaping = false;

    loop {
        let c = proc.accept()?;

        if c == b'\\' {
            escaping = !escaping;
            continue;
        }

        if c == b'`' && !escaping {
            break;
        }

        escaping = false;
    };

    Ok(())
}

pub fn process_script<D: Code>(proc: &Processor<D>) -> HbRes<()> {
    while !proc.match_seq(b"</").matched() {
        if proc.match_seq(b"//").matched() {
            parse_comment_single(proc)?;
        } else if proc.match_seq(b"/*").matched() {
            parse_comment_multi(proc)?;
        } else if proc.match_pred(is_string_delimiter).matched() {
            parse_string(proc)?;
        } else if proc.match_char(b'`').matched() {
            parse_template(proc)?;
        } else {
            proc.accept()?;
        }
    };
    Ok(())
}
