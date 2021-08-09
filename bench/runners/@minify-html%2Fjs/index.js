const fs = require("fs");
const minifyHtml = require("@minify-html/js");
const path = require("path");

const iterations = parseInt(process.env.MHB_ITERATIONS, 10);
const inputDir = process.env.MHB_INPUT_DIR;
const htmlOnly = process.env.MHB_HTML_ONLY === "1";
const outputDir = process.env.MHB_OUTPUT_DIR;

const minifyHtmlCfg = minifyHtml.createConfiguration({
  minify_css: !htmlOnly,
  minify_js: !htmlOnly,
});

const results = fs.readdirSync(inputDir).map((name) => {
  const src = fs.readFileSync(path.join(inputDir, name));

  const out = minifyHtml.minify(src, minifyHtmlCfg);
  const len = out.byteLength;
  if (outputDir) {
    fs.writeFileSync(path.join(outputDir, name), out);
  }

  const start = process.hrtime.bigint();
  for (let i = 0; i < iterations; i++) {
    minifyHtml.minify(src, minifyHtmlCfg);
  }
  const elapsed = process.hrtime.bigint() - start;

  return [name, len, iterations, Number(elapsed) / 1_000_000_000];
});
console.log(JSON.stringify(results));
