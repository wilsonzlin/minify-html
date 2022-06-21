const esbuild = require("esbuild");
const fs = require("fs");
const path = require("path");

const iterations = parseInt(process.env.MHB_ITERATIONS, 10);
const inputDir = process.env.MHB_INPUT_DIR;
const htmlOnly = process.env.MHB_HTML_ONLY === "1";
const outputDir = process.env.MHB_OUTPUT_DIR;

module.exports = {
  htmlOnly,

  esbuildCss: (code, type) => {
    if (type === "inline") {
      code = `x{${code}}`;
    }
    code = esbuild.transformSync(code, {
      charset: "utf8",
      legalComments: "none",
      loader: "css",
      minify: true,
      sourcemap: false,
    }).code;
    if (type === "inline") {
      code = code.trim().slice(2, -1);
    }
    return code;
  },

  esbuildJs: (code) =>
    esbuild.transformSync(code, {
      charset: "utf8",
      legalComments: "none",
      loader: "js",
      minify: true,
      sourcemap: false,
    }).code,

  run: (minifierFn) => {
    console.log(
      JSON.stringify(
        fs.readdirSync(inputDir).map((name) => {
          const src = fs.readFileSync(path.join(inputDir, name));

          const out = minifierFn(src);
          const len = Buffer.from(out).length;
          if (outputDir) {
            fs.writeFileSync(path.join(outputDir, name), out);
          }

          const start = process.hrtime.bigint();
          for (let i = 0; i < iterations; i++) {
            minifierFn(src);
          }
          const elapsed = process.hrtime.bigint() - start;

          return [name, len, iterations, Number(elapsed) / 1_000_000_000];
        })
      )
    );
  },
};
