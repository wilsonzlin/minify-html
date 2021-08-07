const esbuild = require('esbuild');
const htmlMinifier = require('html-minifier');
const minifyHtml = require('@minify-html/js');
const minimize = require('minimize');

const testJsAndCssMinification = process.env.HTML_ONLY !== '1';

const jsMime = new Set([
  undefined,
  'application/ecmascript',
  'application/javascript',
  'application/x-ecmascript',
  'application/x-javascript',
  'text/ecmascript',
  'text/javascript',
  'text/javascript1.0',
  'text/javascript1.1',
  'text/javascript1.2',
  'text/javascript1.3',
  'text/javascript1.4',
  'text/javascript1.5',
  'text/jscript',
  'text/livescript',
  'text/x-ecmascript',
  'text/x-javascript',
]);

const minifyHtmlCfg = minifyHtml.createConfiguration({
  minify_js: testJsAndCssMinification,
  minify_css: testJsAndCssMinification,
});

const esbuildCss = code => esbuild.transformSync(code, {
  loader: "css",
  minify: true,
}).code;
const esbuildJs = code => esbuild.transformSync(code, {
  loader: "js",
  minify: true,
}).code.replace(/<\/script/g, "<\\/script");

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
  minifyCSS: testJsAndCssMinification && esbuildCss,
  minifyJS: testJsAndCssMinification && esbuildJs,
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

module.exports = {
  '@minify-html/js': (_, buffer) => minifyHtml.minify(buffer, minifyHtmlCfg),
  'html-minifier': content => htmlMinifier.minify(content, htmlMinifierCfg),
  'minimize': testJsAndCssMinification
    ? (content) => new minimize({
        plugins: [
          {
            id: 'esbuild',
            element: (node, next) => {
              if (node.type === 'text' && node.parent) {
                if (node.parent.type === 'script' && jsMime.has(node.parent.attribs.type)) {
                  node.data = esbuildJs(node.data);
                } else if (node.parent.type === 'style') {
                  node.data = esbuildCss(node.data);
                }
              }
              next();
            },
          },
        ],
      }).parse(content)
    : content => new minimize().parse(content),
};
