const binaryName = [process.platform, process.arch, process.versions.modules].join('__');

const hyperbuild = require(`./${binaryName}.node`);

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
