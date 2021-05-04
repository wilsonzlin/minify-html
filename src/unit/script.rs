use aho_corasick::{AhoCorasick, AhoCorasickBuilder};
use lazy_static::lazy_static;
use crate::cfg::Cfg;
use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
#[cfg(feature = "js-esbuild")]
use {
    std::sync::Arc,
    esbuild_rs::{TransformOptionsBuilder, TransformOptions},
    minify::json::minify,
    crate::proc::{EsbuildSection, ResultType},
    crate::proc::checkpoint::WriteCheckpoint,
};

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
    if cfg.minify_js {
        let src = start.written_range(proc);
        let (wg, results) = proc.new_esbuild_section();
        if js {
            unsafe {
                esbuild_rs::transform_direct_unmanaged(&proc[src], &TRANSFORM_OPTIONS.clone(), move |result| {
                    let mut guard = results.lock().unwrap();
                    guard.push(EsbuildSection {
                        src,
                        result: ResultType::EsBuildResult(result),
                    });
                    // Drop Arc reference and Mutex guard before marking task as complete as it's possible proc::finish
                    // waiting on WaitGroup will resume before Arc/Mutex is dropped after exiting this function.
                    drop(guard);
                    drop(results);
                    drop(wg);
                });
            };
        } else {
            let raw_json = unsafe { String::from_utf8_unchecked(proc[src].to_vec()) };
            let result = minify(&raw_json[..]);
            let mut guard = results.lock().unwrap();
            guard.push(EsbuildSection {
                src,
                result: ResultType::StringResult(result),
            });
            drop(guard);
            drop(results);
            drop(wg);
        }
    };

    Ok(())
}
