// This wrapper file also exists to allow importing from ESM contexts, as Node.js does not allow importing ".node" modules directly from ESM.

try {
  // Prioritise any local built binary.
  module.exports = require("./index.node");
} catch {
  module.exports = require(`@minify-html/node-${process.platform}-${process.arch}`);
}
