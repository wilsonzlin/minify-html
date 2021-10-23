#!/usr/bin/env node

const lib = require(".");
const fs = require("fs");

const cfgObj = {};
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
      cfgObj[name.replace(/-/g, "_")] = true;
    }
  } else {
    input = arg;
  }
}

const cfg = lib.createConfiguration(cfgObj);
const min = lib.minify(fs.readFileSync(input), cfg);
fs.writeFileSync(output, min);
