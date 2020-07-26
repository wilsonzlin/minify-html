use lazy_static::lazy_static;
use aho_corasick::AhoCorasick;
use crate::cfg::Cfg;
use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
#[cfg(feature = "js-esbuild")]
use {
    std::sync::Arc,
    esbuild_rs::{TransformOptionsBuilder, TransformOptions},
    crate::proc::JsMinSection,
    crate::proc::checkpoint::Checkpoint,
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
    static ref SCRIPT_END: AhoCorasick = AhoCorasick::new(&["</script"]);
}

pub fn process_script(proc: &mut Processor, cfg: &Cfg, js: bool) -> ProcessingResult<()> {
    #[cfg(feature = "js-esbuild")]
    let start = Checkpoint::new(proc);
    proc.require_not_at_end()?;
    proc.m(WhileNotSeq(&SCRIPT_END), Keep);
    // `process_tag` will require closing tag.

    #[cfg(feature = "js-esbuild")]
    if js && cfg.minify_js {
        let (wg, results) = proc.new_script_section();
        let src = start.written_range(proc);
        unsafe {
            esbuild_rs::transform_direct_unmanaged(&proc[src], &TRANSFORM_OPTIONS.clone(), move |result| {
                let mut guard = results.lock().unwrap();
                guard.push(JsMinSection {
                    src,
                    result,
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
