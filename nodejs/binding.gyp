{
  "targets": [
    {
      "target_name": "index",
      "sources": [
        "binding.c"
      ],
      "include_dirs": [
        "native/target/release/",
      ],
      "actions": [
        {
          "action_name": "build minify-html-ffi static library",
          "inputs": ["native/src/lib.rs"],
          "outputs": ["native/target/release/libminify_html_ffi.a", "native/target/release/minify_html_ffi.h"],
          "action": ["cargo", "build", "--manifest-path", "native/Cargo.toml", "--release"],
        },
      ],
      "libraries": [
        "../native/target/release/libminify_html_ffi.a",
      ],
    },
  ],
}
