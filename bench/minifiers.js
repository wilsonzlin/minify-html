const htmlMinifier = require("html-minifier");
const hyperbuild = require("hyperbuild");
const minimize = require("minimize");

module.exports = {
  'hyperbuild-nodejs': content => hyperbuild.minify(content),
  'html-minifier': content => htmlMinifier.minify(content, {
    collapseBooleanAttributes: true,
    collapseInlineTagWhitespace: true,
    collapseWhitespace: true,
    customEventAttributes: [],
    decodeEntities: true,
    ignoreCustomComments: [],
    ignoreCustomFragments: [/<\?[\s\S]*?\?>/],
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
  }),
  'minimize': content => new minimize().parse(content),
};
