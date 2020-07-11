use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;

pub fn process_style(proc: &mut Processor) -> ProcessingResult<()> {
    loop {
        proc.require_not_at_end()?;
        // Use fast memchr.
        proc.m(WhileNotChar(b'<'), Keep);
        // `process_tag` will require closing tag.
        if proc.m(IsSeq(b"</style"), MatchOnly).nonempty() {
            break;
        };
        // Consume '<'.
        proc.accept_expect();
    };
    Ok(())
}
