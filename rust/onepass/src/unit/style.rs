use aho_corasick::{AhoCorasick, AhoCorasickBuilder};
use lazy_static::lazy_static;

#[cfg(feature = "js-esbuild")]
use {
    crate::proc::checkpoint::WriteCheckpoint,
    crate::proc::EsbuildSection,
    esbuild_rs::{Loader, TransformOptions, TransformOptionsBuilder},
    std::sync::Arc,
};

use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
use crate::Cfg;

#[cfg(feature = "js-esbuild")]
lazy_static! {
    static ref TRANSFORM_OPTIONS: Arc<TransformOptions> = {
        let mut builder = TransformOptionsBuilder::new();
        builder.loader = Loader::CSS;
        builder.minify_identifiers = true;
        builder.minify_syntax = true;
        builder.minify_whitespace = true;
        builder.build()
    };
}

lazy_static! {
    static ref STYLE_END: AhoCorasick = AhoCorasickBuilder::new()
        .ascii_case_insensitive(true)
        .build(&["</style"]);
}

#[inline(always)]
pub fn process_style(proc: &mut Processor, cfg: &Cfg) -> ProcessingResult<()> {
    #[cfg(feature = "js-esbuild")]
    let start = WriteCheckpoint::new(proc);
    proc.require_not_at_end()?;
    proc.m(WhileNotSeq(&STYLE_END), Keep);
    // `process_tag` will require closing tag.

    // TODO This is copied from script.rs.
    #[cfg(feature = "js-esbuild")]
    if cfg.minify_css {
        let (wg, results) = proc.new_esbuild_section();
        let src = start.written_range(proc);
        unsafe {
            esbuild_rs::transform_direct_unmanaged(
                &proc[src],
                &TRANSFORM_OPTIONS.clone(),
                move |result| {
                    let mut guard = results.lock().unwrap();
                    // TODO Are there other places that can have unintentional closing tags?
                    let mut escaped = Vec::<u8>::new();
                    // STYLE_END must be case insensitive.
                    STYLE_END.replace_all_with_bytes(
                        result.code.as_str().trim().as_bytes(),
                        &mut escaped,
                        |_, orig, dst| {
                            dst.extend(b"<\\/");
                            // Keep original case.
                            dst.extend(&orig[2..]);
                            true
                        },
                    );
                    guard.push(EsbuildSection { src, escaped });
                    // Drop Arc reference and Mutex guard before marking task as complete as it's possible proc::finish
                    // waiting on WaitGroup will resume before Arc/Mutex is dropped after exiting this function.
                    drop(guard);
                    drop(results);
                    drop(wg);
                },
            );
        };
    };

    Ok(())
}
