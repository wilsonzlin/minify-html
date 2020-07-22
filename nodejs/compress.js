const {readFileSync, writeFileSync} = require('fs');
const {join} = require('path');
const {gzipSync} = require('zlib');

const src = readFileSync(join(__dirname, 'dist', 'index.node'));
const out = gzipSync(src);
writeFileSync(join(__dirname, 'dist', 'index.node.gz'), out);
