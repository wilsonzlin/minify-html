use crate::lex::Lexer;
use crate::spec::tag::ns::Namespace;
use crate::err::HbRes;
use crate::gen::dfa::State;

pub fn parse_tag(lexer: &mut Lexer, ns: Namespace) -> HbRes<()> {
    debug_assert_eq!(lexer.state(), State::OpeningTagStart);

    let name_token = lexer.accumulate_until_next_state()?;
    let name = lexer.keep(name_token);

    loop {
        debug_assert_eq!(lexer.state(), State::OpeningTagWhitespace);
        lexer.accumulate_until_next_state();
        match lexer.state() {
            State::AttrName => {}
            State::Text => return Ok(()),
            _ => unreachable!(),
        };
    };
}
