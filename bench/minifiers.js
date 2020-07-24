const esbuild = require('esbuild');
const htmlMinifier = require('html-minifier');
const minifyHtml = require('@minify-html/js');
const minimize = require('minimize');

const minifyHtmlCfg = minifyHtml.createConfiguration({minifyJs: true});

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

class EsbuildAsync {
  constructor () {
    this.promises = [];
  }

  queue (code) {
    const id = this.promises.push(esbuild.transform(code, {
      minify: true,
      minifyWhitespace: true,
      minifyIdentifiers: true,
      minifySyntax: true,
    })) - 1;
    return `_____ESBUILD_ASYNC_PLACEHOLDER_${id}_____`;
  }

  async finalise (html) {
    const jsTransformResults = await Promise.all(this.promises);
    return html.replace(/_____ESBUILD_ASYNC_PLACEHOLDER_([0-9]+)_____/g, (_, id) => jsTransformResults[id].js.replace(/<\/script/g, '<\\/script'));
  }
}

module.exports = {
  'minify-html-nodejs': (_, buffer) => minifyHtml.minifyInPlace(Buffer.from(buffer), minifyHtmlCfg),
  'html-minifier': async (content) => {
    const js = new EsbuildAsync();
    const res = htmlMinifier.minify(content, {
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
      minifyJS: code => js.queue(code),
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
    });
    return js.finalise(res);
  },
  'minimize': async (content) => {
    const js = new EsbuildAsync();
    const res = new minimize({
      plugins: [{
        id: 'esbuild',
        element: (node, next) => {
          if (node.type === 'text' && node.parent && node.parent.type === 'script' && jsMime.has(node.parent.attribs.type)) {
            node.data = js.queue(node.data);
          }
          next();
        },
      }],
    }).parse(content);
    return js.finalise(res);
  },
};
