const htmlMinifier = require("html-minifier");
const hyperbuild = require("hyperbuild");
const minimize = require("minimize");

module.exports = {
  'hyperbuild-nodejs': (_, buffer) => hyperbuild.minifyInPlace(Buffer.from(buffer), {minifyJs: false}),
  'html-minifier': content => htmlMinifier.minify(content, {
    collapseBooleanAttributes: true,
    collapseInlineTagWhitespace: true,
    collapseWhitespace: true,
    // hyperbuild can do context-aware whitespace removal, which is safe when configured correctly to match how whitespace is used in the document.
    // html-minifier cannot, so whitespace must be collapsed conservatively.
    // Alternatively, hyperbuild can also be made to remove whitespace regardless of context.
    conservativeCollapse: true,
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
