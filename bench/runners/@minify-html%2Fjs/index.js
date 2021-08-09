const minifyHtml = require("@minify-html/js");
const { htmlOnly, run } = require("../common");

const minifyHtmlCfg = minifyHtml.createConfiguration({
  minify_css: !htmlOnly,
  minify_js: !htmlOnly,
});

run((src) => minifyHtml.minify(src, minifyHtmlCfg));
