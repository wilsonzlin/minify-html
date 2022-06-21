const minifyHtml = require("@minify-html/node");
const { htmlOnly, run } = require("../common");

const minifyHtmlCfg = {
  minify_css: !htmlOnly,
  minify_js: !htmlOnly,
};

run((src) => minifyHtml.minify(src, minifyHtmlCfg));
