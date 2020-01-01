const fs = require('fs');
const path = require('path');
const request = require('request');

const binaryPath = path.join(__dirname, "native", "index.node");
const binaryName = [
  require('./package').version,
  '-',
  [process.platform, {
    darwin: 'macos',
    linux: 'linux',
    win32: 'windows',
  }],
  '-',
  [process.arch, {
    x64: 'x86_64',
  }],
  '-node',
  [process.versions.modules, {
    // Sourced from https://nodejs.org/en/download/releases/.
    57: 8,
    59: 9,
    64: 10,
    67: 11,
    72: 12,
    79: 13,
  }],
  '.node',
].map(p => typeof p == 'string'
  ? p
  : p[1][p[0]]
).join('');
const binaryUrl = `https://wilsonl.in/hyperbuild/bin/${binaryName}`;

console.log(`Fetching ${binaryUrl}...`);
request(binaryUrl)
  .on('response', res => {
    if (res.statusCode !== 200) {
      console.error(`Failed to download prebuilt native module with status ${res.statusCode}, will build from source`);
      process.exit(1);
    }
  })
  .on('error', err => {
    console.error(err);
    console.error(`Could not download prebuilt native module, building from source...`);
    process.exit(1);
  })
  .pipe(fs.createWriteStream(binaryPath));
