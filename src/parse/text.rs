use crate::lex::Lexer;
use crate::spec::tag::whitespace::{get_whitespace_minification_for_tag, WhitespaceMinification};
use crate::spec::tag::ns::Namespace;
use crate::err::HbRes;
use crate::gen::dfa::State;
use crate::parse::tag::{parse_opening_tag, parse_tag};

pub fn parse_text(lexer: &mut Lexer, ns: Namespace) -> HbRes<()> {
    let &WhitespaceMinification { collapse, destroy_whole, trim } = get_whitespace_minification_for_tag(parent.map(|r| &proc[r]));

    // The last_write_state when we kept a whitespace token, so only valid when last_write_state is TextWhitespace.
    let mut before_ws = State::Unknown;

    loop {
        debug_assert_eq!(lexer.state(), State::Text);
        let token = lexer.accumulate_until_next_state()?;
        lexer.keep(token);
        match lexer.state() {
            State::ClosingTagName | State::EOF => {
                if trim && lexer.last_write_state() == State::TextWhitespace {
                    lexer.undo_kept();
                };
                return Ok(())
            }
            State::Comment => {
                // We do nothing and discard the comment.
                lexer.accumulate_until_next_state();
            }
            State::OpeningTagName => {
                // If we just wrote whitespace, and there was a closing tag just before the whitespace, then we remove
                // the whitespace if `destroy_whole`.
                if destroy_whole && lexer.last_write_state() == State::TextWhitespace && before_ws == State::ClosingTagName {
                    lexer.undo_kept();
                };
                parse_tag(lexer, ns)?;
            }
            State::TextWhitespace => {
                let ws = lexer.accumulate_until_next_state()?;
                // We'll always write whitespace except at the beginning if `trim`, and then decide at next *TagName if
                // we want to erase it.
                if !(trim && before_ws == State::Unknown) {
                    before_ws = lexer.last_write_state();
                    if collapse {
                        lexer.keep_custom(ws, b" ");
                    } else {
                        lexer.keep(ws);
                    };
                };
            }
            _ => unreachable!(),
        };
    };
}
