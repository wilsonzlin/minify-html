const esbuild = require("esbuild");
const fs = require("fs");
const htmlMinifier = require("html-minifier");
const path = require("path");

const iterations = parseInt(process.env.MHB_ITERATIONS, 10);
const inputDir = process.env.MHB_INPUT_DIR;
const htmlOnly = process.env.MHB_HTML_ONLY === "1";

const htmlMinifierCfg = {
  collapseBooleanAttributes: true,
  collapseInlineTagWhitespace: true,
  collapseWhitespace: true,
  // minify-html can do context-aware whitespace removal, which is safe when configured correctly to match how whitespace is used in the document.
  // html-minifier cannot, so whitespace must be collapsed conservatively.
  // Alternatively, minify-html can also be made to remove whitespace regardless of context.
  conservativeCollapse: true,
  customEventAttributes: [],
  decodeEntities: true,
  ignoreCustomComments: [],
  ignoreCustomFragments: [/<\?[\s\S]*?\?>/],
  minifyCSS: !htmlOnly && esbuildCss,
  minifyJS: !htmlOnly && esbuildJs,
  processConditionalComments: true,
  removeAttributeQuotes: true,
  removeComments: true,
  removeEmptyAttributes: true,
  removeOptionalTags: true,
  removeRedundantAttributes: true,
  removeScriptTypeAttributes: true,
  removeStyleLinkTypeAttributes: true,
  removeTagWhitespace: true,
  useShortDoctype: true,
};

const esbuildCss = (code) =>
  esbuild.transformSync(code, {
    loader: "css",
    minify: true,
  }).code;

const esbuildJs = (code) =>
  esbuild.transformSync(code, {
    loader: "js",
    minify: true,
  }).code;

const results = fs.readdirSync(inputDir).map((name) => {
  const src = fs.readFileSync(path.join(inputDir, name), "utf8");
  const start = process.hrtime.bigint();
  for (let i = 0; i < iterations; i++) {
    htmlMinifier.minify(src, htmlMinifierCfg);
  }
  const elapsed = process.hrtime.bigint() - start;
  return [name, Number(elapsed) / 1_000_000_000];
});
console.log(JSON.stringify(results));
