#[cfg(feature = "js-esbuild")]
use {crossbeam::sync::WaitGroup, esbuild_rs::TransformOptions};

#[cfg(feature = "js-esbuild")]
// TODO The use of WG is ugly and we don't want to be multi-threaded; wait for Rust port esbuild-transform-rs.
pub fn minify_using_esbuild(out: &mut Vec<u8>, code: &[u8], transform_options: &TransformOptions) {
    let wg = WaitGroup::new();
    unsafe {
        let wg = wg.clone();
        // esbuild now officially handles escaping `</script` and `</style`.
        esbuild_rs::transform_direct_unmanaged(code, transform_options, move |result| {
            let min_code = result.code.as_str().trim().as_bytes();
            out.extend_from_slice(min_code);
            drop(wg);
        });
    };
    wg.wait();
}
