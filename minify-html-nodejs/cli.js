#!/usr/bin/env node

const lib = require(".");
const fs = require("fs");

const cfg = {};
let input;
let output;
const args = process.argv.slice(2);
let arg;
while ((arg = args.shift()) !== undefined) {
  if (arg.slice(0, 2) === "--") {
    const name = arg.slice(2);
    if (name === "output") {
      output = args.shift();
    } else {
      cfg[name.replace(/-/g, "_")] = true;
    }
  } else {
    input = arg;
  }
}

const min = lib.minify(fs.readFileSync(input), cfg);
fs.writeFileSync(output, min);
