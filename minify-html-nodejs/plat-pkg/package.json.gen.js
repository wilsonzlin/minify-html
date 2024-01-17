const {platform, arch} = {
  "aarch64-unknown-linux-gnu": {platform: "linux", arch: "arm64"},
  "x86_64-unknown-linux-gnu": {platform: "linux", arch: "x64"},
  "aarch64-apple-darwin": {platform: "darwin", arch: "arm64"},
  "x86_64-apple-darwin": {platform: "darwin", arch: "x64"},
  "x86_64-pc-windows-msvc": {platform: "win32", arch: "x64"},
}[process.env.TARGET];
const {version} = require("../package.json");
require("fs").writeFileSync(`${__dirname}/package.json`, JSON.stringify({
  name: `@minify-html/node-${platform}-${arch}`,
  version,
  description: `@minify-html/node native module build for ${platform}-${arch}`,
  main: "index.node",
  files: ["README.md", "index.node"],
  repository: "wilsonzlin/minify-html",
  os: [platform],
  cpu: [arch],
}, null, 2));
