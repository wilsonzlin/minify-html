const htmlMinifier = require("html-minifier");
const minifyHtml = require("@minify-html/js");
const minimize = require("minimize");
const terser = require('terser');

module.exports = {
  'minify-html-nodejs': (_, buffer) => minifyHtml.minifyInPlace(Buffer.from(buffer), {minifyJs: true}),
  'html-minifier': content => htmlMinifier.minify(content, {
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
    minifyJS: true,
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
  'minimize': content => new minimize({
    plugins: [{
      id: 'terser',
      element: (node, next) => {
        if (node.type === 'text' && node.parent && node.parent.type === 'script') {
          node.data = terser.minify(node.data).code || node.data;
        }
        next();
      },
    }]
  }).parse(content),
};
