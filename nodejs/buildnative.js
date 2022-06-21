const cp = require("child_process");
const path = require("path");

cp.spawnSync(
  "cargo",
  [
    "build",
    "--manifest-path",
    path.join(__dirname, "native", "Cargo.toml"),
    "--release",
  ],
  {
    stdio: "inherit",
  }
);
