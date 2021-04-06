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
      "conditions": [
        ["OS=='mac'", {
          "libraries": [
            "Security.framework",
          ],
        }],
        ["OS!='win'", {
          "libraries": [
            "../native/target/release/libminify_html_ffi.a",
          ],
        }],
        ["OS=='win'", {
          "libraries": [
            "advapi32.lib", "ws2_32.lib", "userenv.lib", "msvcrt.lib",
            "../native/target/release/minify_html_ffi.lib",
          ],
        }],
      ],
    },
  ],
}
