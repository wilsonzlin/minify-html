const request = require('request-promise-native');
const {promises: fs} = require('fs');
const ts = require('typescript');
const path = require('path');

const fromCamelCase = camelCase => camelCase.split(/(?=^|[A-Z])/).map(w => w.toLowerCase());

const BOOLEAN_ATTRS_PATH = path.join(__dirname, '..', 'boolean_attrs.json');
const REDUNDANT_IF_EMPTY_ATTRS_PATH = path.join(__dirname, '..', 'redundant_if_empty_attrs.json');

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

const attrInterfaceToTagName = {
  'anchor': 'a',
};

const reactSpecificAttributes = [
  'defaultChecked', 'defaultValue', 'suppressContentEditableWarning', 'suppressHydrationWarning',
];

const processReactTypeDeclarations = async (source) => {
  const booleanAttributes = new Map();
  const redundantIfEmptyAttributes = new Map();

  const unvisited = [source];
  while (unvisited.length) {
    const node = unvisited.shift();
    if (node.kind === ts.SyntaxKind.InterfaceDeclaration) {
      const name = node.name.escapedText;
      let matches;
      if ((matches = /^([A-Za-z]*)HTMLAttributes/.exec(name))) {
        const tagName = [matches[1].toLowerCase()].map(n => attrInterfaceToTagName[n] || n)[0];
        if (!['all', 'webview'].includes(tagName)) {
          for (const n of node.members.filter(n => n.kind === ts.SyntaxKind.PropertySignature)) {
            // TODO Is escapedText the API for getting name?
            const attr = n.name.escapedText.toLowerCase();
            const types = n.type.kind === ts.SyntaxKind.UnionType
              ? n.type.types.map(t => t.kind)
              : [n.type.kind];
            // If types includes boolean and string, make it a boolean attr to prevent it from being removed if empty value.
            if (types.includes(ts.SyntaxKind.BooleanKeyword)) {
              if (!booleanAttributes.has(attr)) {
                booleanAttributes.set(attr, []);
              }
              booleanAttributes.get(attr).push(tagName);
            } else if (types.includes(ts.SyntaxKind.StringKeyword) || types.includes(ts.SyntaxKind.NumberKeyword)) {
              if (!redundantIfEmptyAttributes.has(attr)) {
                redundantIfEmptyAttributes.set(attr, []);
              }
              redundantIfEmptyAttributes.get(attr).push(tagName);
            }
          }
        }
      }
    }
    // forEachChild doesn't seem to work if return value is number (e.g. Array.prototype.push return value).
    node.forEachChild(c => void unvisited.push(c));
  }

  // Sort output JSON object by property so diffs are clearer.
  await fs.writeFile(BOOLEAN_ATTRS_PATH, JSON.stringify(
    Object.fromEntries([...booleanAttributes.entries()].sort((a, b) => a[0].localeCompare(b[0]))),
    null,
    2,
  ));
  await fs.writeFile(REDUNDANT_IF_EMPTY_ATTRS_PATH, JSON.stringify(
    Object.fromEntries([...redundantIfEmptyAttributes.entries()].sort((a, b) => a[0].localeCompare(b[0]))),
    null,
    2,
  ));
};

(async () => {
  const source = ts.createSourceFile(`react.d.ts`, await fetchReactTypingsSource(), ts.ScriptTarget.ES2019);
  await processReactTypeDeclarations(source);
})();
