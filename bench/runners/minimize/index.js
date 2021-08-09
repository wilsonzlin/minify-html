const esbuild = require("esbuild");
const fs = require("fs");
const minimize = require("minimize");
const path = require("path");

const iterations = parseInt(process.env.MHB_ITERATIONS, 10);
const inputDir = process.env.MHB_INPUT_DIR;
const htmlOnly = process.env.MHB_HTML_ONLY === "1";
const outputDir = process.env.MHB_OUTPUT_DIR;

const jsMime = new Set([
  undefined,
  "application/ecmascript",
  "application/javascript",
  "application/x-ecmascript",
  "application/x-javascript",
  "text/ecmascript",
  "text/javascript",
  "text/javascript1.0",
  "text/javascript1.1",
  "text/javascript1.2",
  "text/javascript1.3",
  "text/javascript1.4",
  "text/javascript1.5",
  "text/jscript",
  "text/livescript",
  "text/x-ecmascript",
  "text/x-javascript",
]);

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

const jsCssPlugin = {
  id: "esbuild",
  element: (node, next) => {
    if (node.type === "text" && node.parent) {
      if (
        node.parent.type === "script" &&
        jsMime.has(node.parent.attribs.type)
      ) {
        node.data = esbuildJs(node.data);
      } else if (node.parent.type === "style") {
        node.data = esbuildCss(node.data);
      }
    }
    next();
  },
};

const plugins = htmlOnly ? [] : [jsCssPlugin];

const minifier = new minimize({ plugins });

const results = fs.readdirSync(inputDir).map((name) => {
  const src = fs.readFileSync(path.join(inputDir, name), "utf8");

  const out = minifier.parse(src);
  const len = Buffer.from(out, "utf8").length;
  if (outputDir) {
    fs.writeFileSync(path.join(outputDir, name), out);
  }

  const start = process.hrtime.bigint();
  for (let i = 0; i < iterations; i++) {
    minifier.parse(src);
  }
  const elapsed = process.hrtime.bigint() - start;

  return [name, len, iterations, Number(elapsed) / 1_000_000_000];
});
console.log(JSON.stringify(results));
