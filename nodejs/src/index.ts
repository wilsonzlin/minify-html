const binaryName = [
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
  : p[1][p[0] as string]
).join('');

const hyperbuild = require(`./${binaryName}`);

export const minify = (code: string): string => {
  const buf = Buffer.from(code);
  const len = hyperbuild.minify(buf);
  return buf.slice(0, len).toString();
};

export const minifyInPlace = (buf: Buffer): Buffer => {
  const len = hyperbuild.minify(buf);
  // This does not do a copy.
  return buf.slice(0, len);
};
