use crate::err::{HbRes, HbErr};
use crate::proc::{Processor};

fn is_string_delimiter(c: u8) -> bool {
    c == b'"' || c == b'\''
}

fn parse_comment_single<'d, 'p>(proc: &'p mut Processor<'d>) -> HbRes<()> {
    cascade_return!(proc.match_seq(b"//").expect().keep());

    // Comment can end at closing </script>.
    // WARNING: Closing tag must not contain whitespace.
    // TODO Optimise
    while !cascade_return!(proc.match_line_terminator().keep().matched()) {
        if cascade_return!(proc.match_seq(b"</script>").matched()) {
            break;
        }

        proc.accept()?;
    }

    Ok(())
}

fn parse_comment_multi<'d, 'p>(proc: &'p mut Processor<'d>) -> HbRes<()> {
    cascade_return!(proc.match_seq(b"/*").expect().keep());

    // Comment can end at closing </script>.
    // WARNING: Closing tag must not contain whitespace.
    // TODO Optimise
    while !cascade_return!(proc.match_seq(b"*/").keep().matched()) {
        if cascade_return!(proc.match_seq(b"</script>").matched()) {
            break;
        }

        proc.accept()?;
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
                return Err(HbErr::ExpectedNotFound("Unterminated JavaScript string"));
            }
        }

        escaping = false;
    };

    Ok(())
}

fn parse_template<'d, 'p>(proc: &'p mut Processor<'d>) -> HbRes<()> {
    cascade_return!(proc.match_char(b'`').expect().keep());

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

pub fn process_script<'d, 'p>(proc: &'p mut Processor<'d>) -> HbRes<()> {
    while !cascade_return!(proc.match_seq(b"</").matched()) {
        if cascade_return!(proc.match_seq(b"//").matched()) {
            parse_comment_single(proc)?;
        } else if cascade_return!(proc.match_seq(b"/*").matched()) {
            parse_comment_multi(proc)?;
        } else if cascade_return!(proc.match_pred(is_string_delimiter).matched()) {
            parse_string(proc)?;
        } else if cascade_return!(proc.match_char(b'`').matched()) {
            parse_template(proc)?;
        } else {
            proc.accept()?;
        }
    };
    Ok(())
}
