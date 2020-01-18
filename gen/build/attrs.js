const request = require('request-promise-native');
const {promises: fs} = require('fs');
const ts = require('typescript');
const path = require('path');

const fromCamelCase = camelCase => camelCase.split(/(?=^|[A-Z])/).map(w => w.toLowerCase());

const ATTRS_PATH = path.join(__dirname, '..', 'attrs.json');

const REACT_TYPINGS_URL = 'https://raw.githubusercontent.com/DefinitelyTyped/DefinitelyTyped/master/types/react/index.d.ts';
const REACT_TYPINGS_FILE = path.join(__dirname, 'react.d.ts');
const fetchReactTypingsSource = async () => {
  try {
    return await fs.readFile(REACT_TYPINGS_FILE, 'utf8');
  } catch (err) {
    if (err.code !== 'ENOENT') {
      throw err;
    }
    const source = await request(REACT_TYPINGS_URL);
    await fs.writeFile(REACT_TYPINGS_FILE, source);
    return source;
  }
};

const tagNameNormalised = {
  'anchor': 'a',
};

const attrNameNormalised = {
  'classname': 'class',
};

const reactSpecificAttributes = [
  'defaultChecked', 'defaultValue', 'suppressContentEditableWarning', 'suppressHydrationWarning',
];

// TODO Consider and check behaviour when value matches case insensitively, after trimming whitespace, numerically (for number values), etc.
// TODO This is currently manually sourced and written. Try to get machine-readable spec and automate.
const defaultAttributeValues = {
  'align': [{
    tags: ['img'],
    defaultValue: 'bottom',
  }],
  'decoding': [{
    tags: ['img'],
    defaultValue: 'auto',
  }],
  'enctype': [{
    tags: ['form'],
    defaultValue: 'application/x-www-form-urlencoded',
  }],
  'frameborder': [{
    tags: ['iframe'],
    defaultValue: '1',
    isPositiveInteger: true,
  }],
  'formenctype': [{
    tags: ['button', 'input'],
    defaultValue: 'application/x-www-form-urlencoded',
  }],
  'height': [{
    tags: ['iframe'],
    defaultValue: '150',
    isPositiveInteger: true,
  }],
  'importance': [{
    tags: ['iframe'],
    defaultValue: 'auto',
  }],
  'loading': [{
    tags: ['iframe', 'img'],
    defaultValue: 'eager',
  }],
  'media': [{
    tags: ['style'],
    defaultValue: 'all',
  }],
  'method': [{
    tags: ['form'],
    defaultValue: 'get',
  }],
  'referrerpolicy': [{
    tags: ['iframe', 'img'],
    defaultValue: 'no-referrer-when-downgrade',
  }],
  'rules': [{
    tags: ['table'],
    defaultValue: 'none',
  }],
  'span': [{
    tags: ['col', 'colgroup'],
    defaultValue: '1',
    isPositiveInteger: true,
  }],
  'target': [{
    tags: ['a', 'form'],
    defaultValue: '_self',
  }],
  'type': [{
    tags: ['button'],
    defaultValue: 'submit',
  }, {
    tags: ['input'],
    defaultValue: 'text',
  }, {
    tags: ['link', 'style'],
    defaultValue: 'text/css',
  }],
  'width': [{
    tags: ['iframe'],
    defaultValue: '300',
    isPositiveInteger: true,
  }],
};

const collapsibleAndTrimmable = {
  'class': [''],
};

// TODO Is escapedText the API for getting name?
const getNameOfNode = n => n.name.escapedText;
const normaliseName = (name, norms) => [name.toLowerCase()].map(n => norms[n] || n)[0];

const processReactTypeDeclarations = async (source) => {
  const nodes = [source];
  // Use index-based loop to keep iterating as nodes array grows.
  for (let i = 0; i < nodes.length; i++) {
    // forEachChild doesn't work if return value is number (e.g. return value of Array.prototype.push).
    nodes[i].forEachChild(c => void nodes.push(c));
  }
  const attributeNodes = nodes
    .filter(n => n.kind === ts.SyntaxKind.InterfaceDeclaration)
    .map(n => [/^([A-Za-z]*)HTMLAttributes/.exec(getNameOfNode(n)), n])
    .filter(([matches]) => matches)
    .map(([matches, node]) => [normaliseName(matches[1], tagNameNormalised), node])
    .filter(([tagName]) => !['all', 'webview'].includes(tagName))
    .sort((a, b) => a[0].localeCompare(b[0]));

  // Process global attributes first as they also appear on some specific tags but we don't want to keep the specific ones if they're global.
  if (attributeNodes[0][0] !== '') {
    throw new Error(`Global attributes is not first to be processed`);
  }

  const attributes = new Map();

  for (const [tagName, node] of attributeNodes) {
    for (const n of node.members.filter(n => n.kind === ts.SyntaxKind.PropertySignature)) {
      const attrName = normaliseName(getNameOfNode(n), attrNameNormalised);
      if (reactSpecificAttributes.includes(attrName)) continue;

      const types = n.type.kind === ts.SyntaxKind.UnionType
        ? n.type.types.map(t => t.kind)
        : [n.type.kind];

      const boolean = types.includes(ts.SyntaxKind.BooleanKeyword);
      // If types includes boolean and string, make it a boolean attr to prevent it from being removed if empty value.
      const redundantIfEmpty = !boolean &&
        (types.includes(ts.SyntaxKind.StringKeyword) || types.includes(ts.SyntaxKind.NumberKeyword));
      const defaultValue = (defaultAttributeValues[attrName] || [])
        .filter(a => a.tags.includes(tagName))
        .map(a => a.defaultValue);
      const collapseAndTrim = (collapsibleAndTrimmable[attrName] || []).includes(tagName);
      if (defaultValue.length > 1) {
        throw new Error(`Tag-attribute combination has multiple default values: ${defaultValue}`);
      }
      const attr = {
        boolean,
        redundant_if_empty: redundantIfEmpty,
        collapse_and_trim: collapseAndTrim,
        default_value: defaultValue[0],
      };

      if (!attributes.has(attrName)) attributes.set(attrName, new Map());
      const tagsForAttribute = attributes.get(attrName);
      if (tagsForAttribute.has(tagName)) throw new Error(`Duplicate tag-attribute combination: <${tagName} ${attrName}>`);

      const globalAttr = tagsForAttribute.get('');
      if (globalAttr) {
        if (globalAttr.boolean !== attr.boolean
          || globalAttr.redundant_if_empty !== attr.redundant_if_empty
          || globalAttr.collapse_and_trim !== attr.collapse_and_trim
          || globalAttr.default_value !== attr.default_value) {
          throw new Error(`Global and tag-specific attributes conflict: ${JSON.stringify(globalAttr, null, 2)} ${JSON.stringify(attr, null, 2)}`);
        }
      } else {
        tagsForAttribute.set(tagName, attr);
      }
    }
  }

  // Sort output JSON object by property so diffs are clearer.
  await fs.writeFile(ATTRS_PATH, JSON.stringify(
    Object.fromEntries(
      [...attributes.entries()]
        .map(([attrName, tagsMap]) => [attrName, Object.fromEntries(
          [...tagsMap.entries()]
            .sort((a, b) => a[0].localeCompare(b[0]))
        )])
        .sort((a, b) => a[0].localeCompare(b[0]))
    ),
    null,
    2,
  ));
};

(async () => {
  const source = ts.createSourceFile(`react.d.ts`, await fetchReactTypingsSource(), ts.ScriptTarget.ES2019);
  await processReactTypeDeclarations(source);
})();
