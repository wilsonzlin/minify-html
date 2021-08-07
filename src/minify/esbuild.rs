#[cfg(feature = "js-esbuild")]
use {aho_corasick::AhoCorasick, crossbeam::sync::WaitGroup, esbuild_rs::TransformOptions};

#[cfg(feature = "js-esbuild")]
// TODO The use of WG is ugly and we don't want to be multi-threaded; wait for Rust port esbuild-transform-rs.
// `tag_to_escape` must be case insensitive.
pub fn minify_using_esbuild(
    out: &mut Vec<u8>,
    code: &[u8],
    transform_options: &TransformOptions,
    tag_to_escape: &'static AhoCorasick,
) {
    let wg = WaitGroup::new();
    unsafe {
        let wg = wg.clone();
        esbuild_rs::transform_direct_unmanaged(code, transform_options, move |result| {
            // TODO (JS) Handle other forms:
            // 1 < /script/.exec(a).length
            // `  ${`  ${a</script/}  `}  `
            // // </script>
            // /* </script>
            // Considerations:
            // - Need to parse strings (e.g. "", '', ``) so syntax within strings aren't mistakenly interpreted as code.
            // - Need to be able to parse regex literals to determine string delimiters aren't actually characters in the regex.
            // - Determining whether a slash is division or regex requires a full-blown JS parser to handle all cases (this is a well-known JS parsing problem).
            // - `/</script` or `/</ script` are not valid JS so don't need to be handled.
            // TODO (CSS) Are there other places that can have unintentional closing tags?
            tag_to_escape.replace_all_with_bytes(
                result.code.as_str().trim().as_bytes(),
                out,
                |_, orig, dst| {
                    dst.extend(b"<\\/");
                    // Keep original case.
                    dst.extend(&orig[2..]);
                    true
                },
            );
            drop(wg);
        });
    };
    wg.wait();
}
