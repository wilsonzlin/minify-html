const native = require(`./native.node`);

export type Configuration = {
  minifyJs: boolean;
};

export const minify = (code: string, cfg: Configuration): string => {
  const buf = Buffer.from(code);
  const len = native.minify(buf, cfg);
  return buf.slice(0, len).toString();
};

export const minifyInPlace = (buf: Buffer, cfg: Configuration): Buffer => {
  const len = native.minify(buf, cfg);
  // This does not do a copy.
  return buf.slice(0, len);
};
