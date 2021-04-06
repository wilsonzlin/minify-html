const cp = require("child_process");
const path = require("path");
const pkg = require("./package.json");

cp.spawnSync(
  "cargo",
  [
    "build",
    "--manifest-path",
    path.join(__dirname, "native", "Cargo.toml"),
    "--release",
    "--features",
    pkg.name.split("/")[1],
  ],
  {
    stdio: "inherit",
  }
);
