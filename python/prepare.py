import shutil
import sys

feature = sys.argv[1]

module_name = "minify_html_core" if feature == "core" else "minify_html"
cargo_file = "Cargo.core.toml" if feature == "core" else "Cargo.js.toml"

with open("src/lib.template.rs", "r") as template:
  actual = template.read().replace("REPLACE_WITH_MODULE_NAME", module_name)
  with open("src/lib.rs", "w") as librs:
    librs.write(actual)

shutil.copyfile(cargo_file, "Cargo.toml")
