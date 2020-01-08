const request = require('request-promise-native');
const {promises: fs} = require('fs');
const ts = require('typescript');
const path = require('path');

const fromCamelCase = camelCase => camelCase.split(/(?=^|[A-Z])/).map(w => w.toLowerCase());

const BOOLEAN_ATTRS_PATH = path.join(__dirname, '..', 'boolean_attrs.json');

const REACT_TYPINGS_URL = 'https://raw.githubusercontent.com/DefinitelyTyped/DefinitelyTyped/master/types/react/index.d.ts';
const REACT_TYPINGS_FILE = path.join(__dirname, 'react.d.ts');
const fetchReactTypingsSource = async () => {
  try {
    return await fs.readFile(REACT_TYPINGS_FILE, "utf8");
  } catch (err) {
    if (err.code !== "ENOENT") {
      throw err;
    }
    const source = await request(REACT_TYPINGS_URL);
    await fs.writeFile(REACT_TYPINGS_FILE, source);
    return source;
  }
};

const processReactTypeDeclarations = async (source) => {
  let tagNameToInterfaceMap;
  let booleanAttributes = new Map();

  const unvisited = [source];
  while (unvisited.length) {
    const node = unvisited.shift();
    let matches;
    switch (node.kind) {
      case ts.SyntaxKind.InterfaceDeclaration:
        const name = node.name.escapedText;
        if (name === "ReactHTML") {
          // Each member of ReactHTML looks something like:
          //
          //   area: DetailedHTMLFactory<AreaHTMLAttributes<HTMLAreaElement>, HTMLAreaElement>;
          //   ^^^^ [1]                                                       ^^^^^^^^^^^^^^^ [2]
          //
          // Get mapping from tag name [1] to interface name [2].
          tagNameToInterfaceMap = Object.fromEntries(node.members.map(m => [m.name.escapedText, m.type.typeArguments[1].typeName.escapedText]));
        } else if ((matches = /^([A-Za-z]+)HTMLAttributes/.exec(name))) {
          const tagName = matches[1].toLowerCase();
          if (!['all', 'webview'].includes(tagName)) {
            node.members
              .filter(n => n.kind === ts.SyntaxKind.PropertySignature)
              .filter(n => n.type.kind === ts.SyntaxKind.BooleanKeyword)
              .map(n => n.name.escapedText)
              .forEach(attr => {
                attr = attr.toLowerCase();
                if (!booleanAttributes.has(attr)) {
                  booleanAttributes.set(attr, []);
                }
                booleanAttributes.get(attr).push(tagName);
              });
          }
        }
        break;
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
};

(async () => {
  const source = ts.createSourceFile(`react.d.ts`, await fetchReactTypingsSource(), ts.ScriptTarget.ES2019);
  await processReactTypeDeclarations(source);
})();
