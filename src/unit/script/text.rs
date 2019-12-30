use crate::err::ProcessingResult;
use crate::proc::Processor;

pub fn process_text_script(proc: &mut Processor) -> ProcessingResult<()> {
    // NOTE: See "notes/Text script content.md".
    let mut in_comment = false;
    let mut comment_has_unclosed_script = false;
    loop {
        // TODO Optimise
        if chain!(proc.match_seq(b"<!--").keep().matched()) {
            // NOTE: Could already be in comment, so don't reset `comment_has_unclosed_script`.
            in_comment = true;
        } else if chain!(proc.match_seq(b"-->").keep().matched()) {
            comment_has_unclosed_script = false;
            in_comment = false;
        } else if in_comment && chain!(proc.match_seq(b"<script").keep().matched()) {
            // TODO DOC Case sensitive, nothing else in tag.
            // TODO Opening tag can have attributes, whitespace, etc.
            chain!(proc.match_char(b'>').require()?.keep());
            comment_has_unclosed_script = true;
        } else if chain!(proc.match_seq(b"</script").matched()) {
            if !comment_has_unclosed_script {
                break;
            }
            comment_has_unclosed_script = false;
            // Keep previously matched closing tag start.
            proc.keep();
            // TODO Close tag can have whitespace.
            chain!(proc.match_char(b'>').require()?.keep());
        } else {
            proc.accept()?;
        };
    };
    Ok(())
}
