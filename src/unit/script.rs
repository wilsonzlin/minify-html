use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
use crate::cfg::Cfg;
#[cfg(feature = "js-esbuild")]
use crate::proc::checkpoint::Checkpoint;

pub fn process_script(proc: &mut Processor, cfg: &Cfg) -> ProcessingResult<()> {
    #[cfg(feature = "js-esbuild")]
    let start = Checkpoint::new(proc);
    loop {
        proc.require_not_at_end()?;
        // Use fast memchr. Unfortunately all characters in "</script>" are common in JS code.
        proc.m(WhileNotChar(b'<'), Keep);
        // `process_tag` will require closing tag.
        if proc.m(IsSeq(b"</script"), MatchOnly).nonempty() {
            #[cfg(feature = "js-esbuild")]
            if cfg.minify_js {
                let src_range = start.written_range(proc);
                let src = unsafe {
                    std::string::String::from_utf8_unchecked(proc[src_range].to_vec())
                };
                let min = esbuild_rs::esbuild(&src).trim().as_bytes();
                // `src.len()` is amount of bytes, so this is guaranteed to not overwrite.
                if min.len() < src.len() {
                    start.erase_written(proc);
                    proc.write_slice(min);
                    return Ok(());
                };
            };
            break;
        };
        // Consume '<'.
        proc.accept_expect();
    };
    Ok(())
}
