use lazy_static::lazy_static;
use regex::bytes::Regex;
use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
use esbuild_rs::esbuild;
use crate::cfg::Cfg;
use std::string::String;

static SCRIPT_END_STR: &'static str = "</script";

lazy_static! {
    static ref SCRIPT_END: Regex = Regex::new(SCRIPT_END_STR).unwrap();
}

pub fn process_script(proc: &mut Processor, cfg: &Cfg) -> ProcessingResult<()> {
    // `process_tag` will require closing tag.
    let code = proc.m(WhileNotPat(&SCRIPT_END, SCRIPT_END_STR.len()), Discard);
    if cfg.minify_js {
        let code_str = unsafe { String::from_utf8_unchecked(proc[code].to_vec()) };
        let min = esbuild(&code_str).trim().as_bytes();
        if min.len() < code.len() {
            proc.write_slice(min);
            return Ok(());
        };
    };
    proc.write_range(code);
    Ok(())
}
