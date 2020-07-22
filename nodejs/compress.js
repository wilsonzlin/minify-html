const {readFileSync, writeFileSync} = require('fs');
const {join} = require('path');
const {gzipSync} = require('zlib');

const src = readFileSync(join(__dirname, 'index.node'));
const out = gzipSync(src);
writeFileSync(join(__dirname, 'index.node.gz'), out);
