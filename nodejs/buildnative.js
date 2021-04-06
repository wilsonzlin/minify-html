const cp = require("child_process");
const pkg = require("./package.json");

cp.spawnSync(
  "cargo",
  [
    "build",
    "--manifest-path",
    "native/Cargo.toml",
    "--release",
    "--features",
    pkg.name.split("/")[1],
  ],
  {
    stdio: "inherit",
  }
);
