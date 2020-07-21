use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
#[cfg(feature = "js-esbuild")]
use crate::proc::JsMinSection;
use crate::cfg::Cfg;
#[cfg(feature = "js-esbuild")]
use crate::proc::checkpoint::Checkpoint;
#[cfg(feature = "js-esbuild")]
use esbuild_rs::{TransformOptionsBuilder, TransformOptions};
#[cfg(feature = "js-esbuild")]
use std::sync::Arc;
#[cfg(feature = "js-esbuild")]
use lazy_static::lazy_static;

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
                let (wg, results) = proc.new_script_section();
                let src_range = start.written_range(proc);
                // TODO Optimise: Avoid copying to new Vec.
                let src = Arc::new(proc[src_range].to_vec());
                esbuild_rs::transform(src, TRANSFORM_OPTIONS.clone(), move |result| {
                    results.lock().unwrap().push(JsMinSection {
                        src_range,
                        result,
                    });
                    drop(wg);
                });
                return Ok(());
            };
            break;
        };
        // Consume '<'.
        proc.accept_expect();
    };
    Ok(())
}
