use crate::err::{ErrorType, ProcessingResult};
use crate::proc::{Processor, ProcessorRange};
use crate::spec::codepoint::{is_whitespace, is_digit, is_hex_digit, is_alphanumeric};
use phf::{Set, phf_set};
use crate::pattern::{ITrieNode, TrieLeafNode};

include!(concat!(env!("OUT_DIR"), "/gen_trie_JS_PUNCTUATORS.rs"));

static IF_WHILE_FOR_WITH: Set<&'static [u8]> = phf_set! {
    b"for",
    b"if",
    b"while",
    b"with",
};

#[derive(Copy, Clone)]
enum Syntax {
    StartOfCode,
    Punctuator,
    IfWhileForWithParentheses,
    GroupingParentheses,
    LiteralStringOrTemplate,
    LiteralNumber,
    LiteralRegExp,
    // Keyword, identifier, or null/boolean literal.
    Name(ProcessorRange),
}

fn is_string_delimiter(c: u8) -> bool {
    c == b'"' || c == b'\''
}

fn is_number_exponent_indicator(c: u8) -> bool {
    c == b'e' || c == b'E'
}

fn is_number_exponent_sign(c: u8) -> bool {
    c == b'+' || c == b'-'
}

fn is_name_continuation(c: u8) -> bool {
    // TODO Doc
    // TODO This assumes that name starts with valid.
    // TODO This does not follow spec.
    is_alphanumeric(c) || c == b'$' || c == b'_'
}

fn parse_literal_number(proc: &mut Processor) -> ProcessingResult<()> {
    if chain!(proc.match_char(b'0').keep().matched()) {
        match proc.peek()? {
            b'b' | b'B' | b'o' | b'O' | b'x' | b'X' => {
                // TODO Doc
                // Assume it's valid number and use `is_hex_digit` which works for all.
                proc.accept_expect();
                chain!(proc.match_while_pred(is_hex_digit).keep());
                return Ok(());
            }
            _ => {}
        };
    };
    chain!(proc.match_while_pred(is_digit).keep());
    chain!(proc.match_char(b'.').keep());
    chain!(proc.match_while_pred(is_digit).keep());
    chain!(proc.match_pred(is_number_exponent_indicator).keep());
    chain!(proc.match_pred(is_number_exponent_sign).keep());
    chain!(proc.match_while_pred(is_digit).keep());
    Ok(())
}

fn parse_regex(proc: &mut Processor) -> ProcessingResult<()> {
    if cfg!(debug_assertions) {
        chain!(proc.match_char(b'/').expect().keep());
    } else {
        proc.accept_expect();
    };

    let mut escaping = false;
    let mut inside_class = false;

    loop {
        let c = proc.accept()?;

        if c == b'\\' {
            // If already escaping, then ignore backslash (interpret literally) and continue.
            // If not, then escape next character.
            escaping = !escaping;
            continue;
        };

        // If escaping, then none of these special characters matter.
        if !escaping {
            match (c, inside_class) {
                (b']', true) => inside_class = false,
                (b'[', false) => inside_class = true,
                (b'/', false) => break,
                _ => (),
            };
        } else {
            escaping = false;
        };
    };

    Ok(())
}

fn parse_comment_single(proc: &mut Processor) -> ProcessingResult<()> {
    if cfg!(debug_assertions) {
        chain!(proc.match_seq(b"//").expect().discard());
    } else {
        proc.skip_amount_expect(2);
    };

    // Comment can end at closing </script>.
    // TODO Optimise
    while !chain!(proc.match_line_terminator().discard().matched()) {
        if chain!(proc.match_seq(b"</script>").matched()) {
            break;
        };

        proc.skip()?;
    };

    Ok(())
}

fn parse_comment_multi(proc: &mut Processor) -> ProcessingResult<()> {
    if cfg!(debug_assertions) {
        chain!(proc.match_seq(b"/*").expect().discard());
    } else {
        proc.skip_amount_expect(2);
    };

    // Comment can end at closing </script>.
    // TODO Optimise
    while !chain!(proc.match_seq(b"*/").discard().matched()) {
        if chain!(proc.match_seq(b"</script>").matched()) {
            break;
        };

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
        };

        if !escaping {
            if c == delim {
                break;
            };
            // We've already accepted char, so we can't use proc.match_line_terminator.
            if c == b'\r' || c == b'\n' {
                return Err(ErrorType::UnterminatedJsString);
            };
        } else {
            escaping = false;
        };
    };

    Ok(())
}

fn parse_template(proc: &mut Processor) -> ProcessingResult<()> {
    if cfg!(debug_assertions) {
        chain!(proc.match_char(b'`').expect().keep());
    } else {
        proc.accept_expect();
    };

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

pub fn process_js_script(proc: &mut Processor) -> ProcessingResult<()> {
    chain!(proc.match_while_pred(is_whitespace).discard());
    // This variable is used so that trailing whitespace is simply trimmed/removed instead of collapsed.
    let mut discarded_whitespace = false;
    // Only updated when currently inside parentheses `()` directly after one of these keywords:
    // - if (...)
    // - while (...) // Note that this includes `do {...} while (...)`.
    // - for (...)
    // - with (...)
    let mut parenthesis_depth = 0usize;
    let mut last_syntax: Syntax = Syntax::StartOfCode;
    // Cannot just break on match "</" as that could be "</a/.exec(str)?.length".
    while !chain!(proc.match_seq(b"</script").matched()) {
        if discarded_whitespace {
            proc.write(b' ');
            discarded_whitespace = false;
        };
        match proc.peek_offset(0)? {
            c if is_whitespace(c) => {
                chain!(proc.match_while_pred(is_whitespace).discard());
                discarded_whitespace = true;
            }
            b'.' => {
                if is_digit(proc.peek_offset(1)?) {
                    // Is numeric literal starting with decimal dot.
                    parse_literal_number(proc)?;
                    last_syntax = Syntax::LiteralNumber;
                } else {
                    // Is dot operator.
                    proc.accept_expect();
                    last_syntax = Syntax::Punctuator;
                };
            }
            b'(' => {
                proc.accept_expect();
                if parenthesis_depth > 0 || match last_syntax {
                    Syntax::Name(r) => IF_WHILE_FOR_WITH.contains(&proc[r]),
                    _ => false,
                } {
                    parenthesis_depth += 1;
                };
                last_syntax = Syntax::Punctuator;
            }
            b')' => {
                proc.accept_expect();
                last_syntax = Syntax::GroupingParentheses;
                if parenthesis_depth > 0 {
                    parenthesis_depth -= 1;
                    if parenthesis_depth == 0 {
                        last_syntax = Syntax::IfWhileForWithParentheses;
                    };
                };
            }
            c if is_digit(c) => {
                parse_literal_number(proc)?;
                last_syntax = Syntax::LiteralNumber;
            }
            b'/' => match proc.peek_offset(1)? {
                b'/' => parse_comment_single(proc)?,
                b'*' => parse_comment_multi(proc)?,
                _ => {
                    let is_regex = match last_syntax {
                        Syntax::IfWhileForWithParentheses => true,
                        Syntax::Punctuator => true,
                        Syntax::Name(val) => !proc[val].eq(b"this"),
                        _ => false,
                    };
                    if is_regex {
                        parse_regex(proc)?;
                        last_syntax = Syntax::LiteralRegExp;
                    } else {
                        // Is divide operator.
                        proc.accept_expect();
                        last_syntax = Syntax::Punctuator;
                    };
                }
            }
            c if is_string_delimiter(c) => {
                parse_string(proc)?;
                last_syntax = Syntax::LiteralStringOrTemplate;
            }
            b'`' => {
                parse_template(proc)?;
                last_syntax = Syntax::LiteralStringOrTemplate;
            }
            _ => {
                if chain!(proc.match_trie(JS_PUNCTUATORS).keep().matched()) {
                    last_syntax = Syntax::Punctuator;
                } else {
                    last_syntax = Syntax::Name(chain!(proc.match_while_pred(is_name_continuation).require_with_reason("JavaScript")?.keep().out_range()));
                };
            }
        };
    };
    Ok(())
}
