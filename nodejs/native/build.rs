use std::env;

fn main() {
    let crate_dir = env!("CARGO_MANIFEST_DIR");
    let profile = env::var("PROFILE").unwrap();

    cbindgen::Builder::new()
        .with_language(cbindgen::Language::C)
        .with_crate(crate_dir)
        .generate()
        .expect("generate C bindings")
        .write_to_file(format!("target/{}/minify_html_ffi.h", profile));
}
