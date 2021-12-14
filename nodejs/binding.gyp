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
          "outputs": [
            "native/target/release/libminify_html_ffi.a",
            "native/target/release/minify_html_ffi.h",
            "native/target/release/minify_html_ffi.lib",
          ],
          "action": ["node", "./buildnative.js"],
        }
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
            "kernel32.lib", "ws2_32.lib", "bcrypt.lib", "advapi32.lib", "userenv.lib", "kernel32.lib", "msvcrt.lib",
            "../native/target/release/minify_html_ffi.lib",
          ],
        }],
      ],
    },
  ],
}
