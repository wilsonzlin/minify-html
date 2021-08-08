use aho_corasick::{AhoCorasick, AhoCorasickBuilder};
use lazy_static::lazy_static;

#[cfg(feature = "js-esbuild")]
use {
    crate::proc::checkpoint::WriteCheckpoint,
    crate::proc::EsbuildSection,
    esbuild_rs::{TransformOptions, TransformOptionsBuilder},
    std::sync::Arc,
};

use crate::cfg::Cfg;
use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;

#[cfg(feature = "js-esbuild")]
lazy_static! {
    static ref TRANSFORM_OPTIONS: Arc<TransformOptions> = {
        let mut builder = TransformOptionsBuilder::new();
        builder.minify_identifiers = true;
        builder.minify_syntax = true;
        builder.minify_whitespace = true;
        builder.build()
    };
}

lazy_static! {
    static ref SCRIPT_END: AhoCorasick = AhoCorasickBuilder::new().ascii_case_insensitive(true).build(&["</script"]);
}

#[inline(always)]
pub fn process_script(proc: &mut Processor, cfg: &Cfg, js: bool) -> ProcessingResult<()> {
    #[cfg(feature = "js-esbuild")]
        let start = WriteCheckpoint::new(proc);
    proc.require_not_at_end()?;
    proc.m(WhileNotSeq(&SCRIPT_END), Keep);
    // `process_tag` will require closing tag.

    // TODO This is copied from style.rs.
    #[cfg(feature = "js-esbuild")]
    if js && cfg.minify_js {
        let (wg, results) = proc.new_esbuild_section();
        let src = start.written_range(proc);
        unsafe {
            esbuild_rs::transform_direct_unmanaged(&proc[src], &TRANSFORM_OPTIONS.clone(), move |result| {
                let mut guard = results.lock().unwrap();
                // TODO Handle other forms:
                // 1 < /script/.exec(a).length
                // `  ${`  ${a</script/}  `}  `
                // // </script>
                // /* </script>
                // Considerations:
                // - Need to parse strings (e.g. "", '', ``) so syntax within strings aren't mistakenly interpreted as code.
                // - Need to be able to parse regex literals to determine string delimiters aren't actually characters in the regex.
                // - Determining whether a slash is division or regex requires a full-blown JS parser to handle all cases (this is a well-known JS parsing problem).
                // - `/</script` or `/</ script` are not valid JS so don't need to be handled.
                let mut escaped = Vec::<u8>::new();
                // SCRIPT_END must be case insensitive.
                SCRIPT_END.replace_all_with_bytes(
                    result.code.as_str().trim().as_bytes(),
                    &mut escaped,
                    |_, orig, dst| {
                        dst.extend(b"<\\/");
                        // Keep original case.
                        dst.extend(&orig[2..]);
                        true
                    },
                );
                guard.push(EsbuildSection {
                    src,
                    escaped,
                });
                // Drop Arc reference and Mutex guard before marking task as complete as it's possible proc::finish
                // waiting on WaitGroup will resume before Arc/Mutex is dropped after exiting this function.
                drop(guard);
                drop(results);
                drop(wg);
            });
        };
    };

    Ok(())
}
