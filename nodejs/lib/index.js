const hyperbuild = require('../native');

module.exports = {
  minify: code => {
    const buf = Buffer.from(code);
    const len = hyperbuild.minify(buf);
    return buf.slice(0, len).toString();
  },
  minify_in_place: buf => {
    const len = hyperbuild.minify(buf);
    // This does not do a copy.
    return buf.slice(0, len);
  },
};
