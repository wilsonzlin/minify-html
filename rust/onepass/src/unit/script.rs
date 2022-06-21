use aho_corasick::{AhoCorasick, AhoCorasickBuilder};
use lazy_static::lazy_static;

use crate::cfg::Cfg;
use crate::err::ProcessingResult;
use crate::proc::checkpoint::WriteCheckpoint;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;

lazy_static! {
    static ref SCRIPT_END: AhoCorasick = AhoCorasickBuilder::new()
        .ascii_case_insensitive(true)
        .build(&["</script"]);
}

#[inline(always)]
pub fn process_script(proc: &mut Processor, cfg: &Cfg, js: bool) -> ProcessingResult<()> {
    let start = WriteCheckpoint::new(proc);
    proc.require_not_at_end()?;
    let src = proc.m(WhileNotSeq(&SCRIPT_END), Discard);
    // `process_tag` will require closing tag.

    if js && cfg.minify_js {
        // TODO Write to `out` directly, but only if we can guarantee that the length will never exceed the input.
        let mut output = Vec::new();
        let result = minify_js::minify(proc[src].to_vec(), &mut output);
        // TODO Collect error as warning.
        if !result.is_err() && output.len() < src.len() {
            proc.write_slice(output.as_slice());
        } else {
            proc.write_range(src);
        };
    } else {
        proc.write_range(src);
    };

    Ok(())
}
