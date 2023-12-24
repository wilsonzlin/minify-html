const minifyHtml = require("@minify-html/node");
const { htmlOnly, run } = require("../common");

const minifyHtmlCfg = {
  minify_css: !htmlOnly,
  minify_js: !htmlOnly,
  allow_noncompliant_unquoted_attribute_values: true,
  allow_optimal_entities: true,
  allow_removing_spaces_between_attributes: true,
  minify_doctype: true,
};

run((src) => minifyHtml.minify(src, minifyHtmlCfg));
