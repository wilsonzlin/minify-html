// This wrapper file exists to allow importing from ESM contexts, as Node.js does not allow importing ".node" modules directly from ESM.

module.exports = require("./index.node");
