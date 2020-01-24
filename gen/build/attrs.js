const request = require('request-promise-native');
const {promises: fs} = require('fs');
const ts = require('typescript');
const path = require('path');

const compareEntryNames = (a, b) => a[0].localeCompare(b[0]);
const deepObjectifyMap = map => Object.fromEntries(
  [...map.entries()]
    .map(([key, value]) => [key, value instanceof Map ? deepObjectifyMap(value) : value])
    .sort(compareEntryNames)
);
const fromCamelCase = camelCase => camelCase.split(/(?=^|[A-Z])/).map(w => w.toLowerCase());
const prettyjson = v => JSON.stringify(v, null, 2);

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
    tags: ['html:img'],
    defaultValue: 'bottom',
  }],
  'decoding': [{
    tags: ['html:img'],
    defaultValue: 'auto',
  }],
  'enctype': [{
    tags: ['html:form'],
    defaultValue: 'application/x-www-form-urlencoded',
  }],
  'frameborder': [{
    tags: ['html:iframe'],
    defaultValue: '1',
    isPositiveInteger: true,
  }],
  'formenctype': [{
    tags: ['html:button', 'html:input'],
    defaultValue: 'application/x-www-form-urlencoded',
  }],
  'height': [{
    tags: ['html:iframe'],
    defaultValue: '150',
    isPositiveInteger: true,
  }],
  'importance': [{
    tags: ['html:iframe'],
    defaultValue: 'auto',
  }],
  'loading': [{
    tags: ['html:iframe', 'html:img'],
    defaultValue: 'eager',
  }],
  'media': [{
    tags: ['html:style'],
    defaultValue: 'all',
  }],
  'method': [{
    tags: ['html:form'],
    defaultValue: 'get',
  }],
  'referrerpolicy': [{
    tags: ['html:iframe', 'html:img'],
    defaultValue: 'no-referrer-when-downgrade',
  }],
  'rules': [{
    tags: ['html:table'],
    defaultValue: 'none',
  }],
  'shape': [{
    tags: ['html:area'],
    defaultValue: 'rect',
  }],
  'span': [{
    tags: ['html:col', 'html:colgroup'],
    defaultValue: '1',
    isPositiveInteger: true,
  }],
  'target': [{
    tags: ['html:a', 'html:form'],
    defaultValue: '_self',
  }],
  'type': [{
    tags: ['html:button'],
    defaultValue: 'submit',
  }, {
    tags: ['html:input'],
    defaultValue: 'text',
  }, {
    tags: ['html:link', 'html:style'],
    defaultValue: 'text/css',
  }],
  'width': [{
    tags: ['html:iframe'],
    defaultValue: '300',
    isPositiveInteger: true,
  }]
};

const collapsibleAndTrimmable = {
  'class': ['html:*'],
  'd': ['svg:*'],
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
    .map(n => [/^([A-Za-z]*)(HTML|SVG)Attributes/.exec(getNameOfNode(n)), n])
    .filter(([matches]) => matches)
    .map(([matches, node]) => [matches[2].toLowerCase(), normaliseName(matches[1], tagNameNormalised), node])
    .filter(([namespace, tagName]) => namespace !== 'html' || !['all', 'webview'].includes(tagName))
    .map(([namespace, tag, node]) => ({namespace, tag, node}))
    .sort((a, b) => a.namespace.localeCompare(b.namespace) || a.tag.localeCompare(b.tag));

  // Process global HTML attributes first as they also appear on some specific HTML tags but we don't want to keep the specific ones if they're global.
  if (attributeNodes[0].namespace !== 'html' || attributeNodes[0].tag !== '') {
    throw new Error(`Global HTML attributes is not first to be processed`);
  }

  // Map structure: attr => namespace => tag => config.
  const attributes = new Map();

  for (const {namespace, tag, node} of attributeNodes) {
    const fullyQualifiedTagName = [namespace, tag || '*'].join(':');
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
        .filter(a => a.tags.includes(fullyQualifiedTagName))
        .map(a => a.defaultValue);
      const collapseAndTrim = (collapsibleAndTrimmable[attrName] || []).includes(fullyQualifiedTagName);
      if (defaultValue.length > 1) {
        throw new Error(`Tag-attribute combination <${fullyQualifiedTagName} ${attrName}> has multiple default values: ${defaultValue}`);
      }
      const attr = {
        boolean,
        redundant_if_empty: redundantIfEmpty,
        collapse_and_trim: collapseAndTrim,
        default_value: defaultValue[0],
      };

      if (!attributes.has(attrName)) attributes.set(attrName, new Map());
      const namespacesForAttribute = attributes.get(attrName);
      if (!namespacesForAttribute.has(namespace)) namespacesForAttribute.set(namespace, new Map());
      const tagsForNSAttribute = namespacesForAttribute.get(namespace);
      if (tagsForNSAttribute.has(tag)) throw new Error(`Duplicate tag-attribute combination: <${fullyQualifiedTagName} ${attrName}>`);

      const globalAttr = tagsForNSAttribute.get('*');
      if (globalAttr) {
        if (globalAttr.boolean !== attr.boolean
          || globalAttr.redundant_if_empty !== attr.redundant_if_empty
          || globalAttr.collapse_and_trim !== attr.collapse_and_trim
          || globalAttr.default_value !== attr.default_value) {
          throw new Error(`Global and tag-specific attributes conflict: ${prettyjson(globalAttr)} ${prettyjson(attr)}`);
        }
      } else {
        tagsForNSAttribute.set(tag || '*', attr);
      }
    }
  }

  // Sort output JSON object by property so diffs are clearer.
  await fs.writeFile(ATTRS_PATH, prettyjson(deepObjectifyMap(attributes)));
};

(async () => {
  const source = ts.createSourceFile(`react.d.ts`, await fetchReactTypingsSource(), ts.ScriptTarget.ES2019);
  await processReactTypeDeclarations(source);
})();
