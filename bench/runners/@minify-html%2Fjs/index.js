const fs = require("fs");
const minifyHtml = require("@minify-html/js");
const path = require("path");

const iterations = parseInt(process.env.MHB_ITERATIONS, 10);
const inputDir = process.env.MHB_INPUT_DIR;
const htmlOnly = process.env.MHB_HTML_ONLY === "1";

const minifyHtmlCfg = minifyHtml.createConfiguration({
  minify_css: !htmlOnly,
  minify_js: !htmlOnly,
});

const results = fs.readdirSync(inputDir).map((name) => {
  const src = fs.readFileSync(path.join(inputDir, name));
  const start = process.hrtime.bigint();
  let len;
  for (let i = 0; i < iterations; i++) {
    len = minifyHtml.minify(src, minifyHtmlCfg).byteLength;
  }
  const elapsed = process.hrtime.bigint() - start;
  return [name, len, iterations, Number(elapsed) / 1_000_000_000];
});
console.log(JSON.stringify(results));
