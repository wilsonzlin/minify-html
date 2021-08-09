const minimize = require("minimize");
const { htmlOnly, esbuildCss, esbuildJs, run } = require("../common");

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

run((src) => minifier.parse(src));
