import {readFileSync, writeFileSync} from 'fs';
import ts, {Node, SourceFile, SyntaxKind, Type} from 'typescript';
import {join} from 'path';
import {DATA_DIR, prettyJson, RUST_OUT_DIR} from './_common';

const reactDeclarations = readFileSync(join(__dirname, 'data', 'react.d.ts'), 'utf8');

// TODO Consider and check behaviour when value matches case insensitively, after trimming whitespace, numerically (for number values), etc.
// TODO This file is currently manually sourced and written. Try to get machine-readable spec and automate.
const defaultAttributeValues: {
  [attr: string]: {
    tags: string[];
    defaultValue: string;
    isPositiveInteger?: boolean;
  }[];
} = JSON.parse(readFileSync(join(DATA_DIR, 'attrs.json'), 'utf8'));

const tagNameNormalised = {
  'anchor': 'a',
};

const attrNameNormalised = {
  'classname': 'class',
};

const reactSpecificAttributes = [
  'defaultChecked', 'defaultValue', 'suppressContentEditableWarning', 'suppressHydrationWarning',
];

const collapsibleAndTrimmable = {
  'class': ['html:*'],
  'd': ['svg:*'],
};

// TODO Is escapedText the API for getting name?
const getNameOfNode = (n: any) => n.name.escapedText;
const normaliseName = (name: string, norms: { [name: string]: string }) => [name.toLowerCase()].map(n => norms[n] || n)[0];

type AttrConfig = {
  boolean: boolean;
  redundantIfEmpty: boolean;
  collapseAndTrim: boolean;
  defaultValue?: string;
};

const rsTagAttr = ({
  redundantIfEmpty,
  defaultValue,
  collapseAndTrim,
  boolean,
}: AttrConfig) => `AttributeMinification { boolean: ${boolean}, redundant_if_empty: ${redundantIfEmpty}, collapse_and_trim: ${collapseAndTrim}, default_value: ${defaultValue == undefined ? 'None' : `Some(b"${defaultValue}")`} }`;

const processReactTypeDeclarations = (source: SourceFile) => {
  const nodes: Node[] = [source];
  // Use index-based loop to keep iterating as nodes array grows.
  for (let i = 0; i < nodes.length; i++) {
    // forEachChild doesn't work if return value is number (e.g. return value of Array.prototype.push).
    nodes[i].forEachChild(c => void nodes.push(c));
  }
  const attributeNodes = nodes
    .filter(n => n.kind === ts.SyntaxKind.InterfaceDeclaration)
    .map(n => [/^([A-Za-z]*)(HTML|SVG)Attributes/.exec(getNameOfNode(n)), n])
    .filter(([matches]) => !!matches)
    .map(([matches, node]) => [matches![2].toLowerCase(), normaliseName(matches![1], tagNameNormalised), node])
    .filter(([namespace, tagName]) => namespace !== 'html' || !['all', 'webview'].includes(tagName))
    .map(([namespace, tag, node]) => ({namespace, tag, node}))
    .sort((a, b) => a.namespace.localeCompare(b.namespace) || a.tag.localeCompare(b.tag));

  // Process global HTML attributes first as they also appear on some specific HTML tags but we don't want to keep the specific ones if they're global.
  if (attributeNodes[0].namespace !== 'html' || attributeNodes[0].tag !== '') {
    throw new Error(`Global HTML attributes is not first to be processed`);
  }

  // Map structure: attr => namespace => tag => config.
  const attributes = new Map<string, Map<'html' | 'svg', Map<string, AttrConfig>>>();

  for (const {namespace, tag, node} of attributeNodes) {
    const fullyQualifiedTagName = [namespace, tag || '*'].join(':');
    for (const n of node.members.filter((n: Node) => n.kind === ts.SyntaxKind.PropertySignature)) {
      const attrName = normaliseName(getNameOfNode(n), attrNameNormalised);
      if (reactSpecificAttributes.includes(attrName)) continue;

      const types: SyntaxKind[] = n.type.kind === ts.SyntaxKind.UnionType
        ? n.type.types.map((t: Node) => t.kind)
        : [n.type.kind];

      const boolean = types.includes(ts.SyntaxKind.BooleanKeyword);
      // If types includes boolean and string, make it a boolean attr to prevent it from being removed if empty value.
      const redundantIfEmpty = !boolean && types.some(t => t === ts.SyntaxKind.StringKeyword || t === ts.SyntaxKind.NumberKeyword);
      const defaultValues = (defaultAttributeValues[attrName] || [])
        .filter(a => a.tags.includes(fullyQualifiedTagName))
        .map(a => a.defaultValue);
      const collapseAndTrim = (collapsibleAndTrimmable[attrName] || []).includes(fullyQualifiedTagName);
      if (defaultValues.length > 1) {
        throw new Error(`Tag-attribute combination <${fullyQualifiedTagName} ${attrName}> has multiple default values: ${defaultValues}`);
      }
      const attr: AttrConfig = {
        boolean,
        redundantIfEmpty,
        collapseAndTrim,
        defaultValue: defaultValues[0],
      };

      if (!attributes.has(attrName)) attributes.set(attrName, new Map());
      const namespacesForAttribute = attributes.get(attrName)!;
      if (!namespacesForAttribute.has(namespace)) namespacesForAttribute.set(namespace, new Map());
      const tagsForNsAttribute = namespacesForAttribute.get(namespace)!;
      if (tagsForNsAttribute.has(tag)) throw new Error(`Duplicate tag-attribute combination: <${fullyQualifiedTagName} ${attrName}>`);

      const globalAttr = tagsForNsAttribute.get('*');
      if (globalAttr) {
        if (globalAttr.boolean !== attr.boolean
          || globalAttr.redundantIfEmpty !== attr.redundantIfEmpty
          || globalAttr.collapseAndTrim !== attr.collapseAndTrim
          || globalAttr.defaultValue !== attr.defaultValue) {
          throw new Error(`Global and tag-specific attributes conflict: ${prettyJson(globalAttr)} ${prettyJson(attr)}`);
        }
      } else {
        tagsForNsAttribute.set(tag || '*', attr);
      }
    }
  }

  let code = `
use lazy_static::lazy_static;
use std::collections::HashMap;
use crate::spec::tag::ns::Namespace;

pub struct AttributeMinification {
    pub boolean: bool,
    pub redundant_if_empty: bool,
    pub collapse_and_trim: bool,
    pub default_value: Option<&'static [u8]>,
}

pub enum AttrMapEntry {
    AllNamespaceElements(AttributeMinification),
    SpecificNamespaceElements(HashMap<&'static [u8], AttributeMinification>),
}

pub struct ByNamespace {
    // Make pub so this struct can be statically created in gen/attrs.rs.
    pub html: Option<AttrMapEntry>,
    pub svg: Option<AttrMapEntry>,
}

impl ByNamespace {
    #[inline(always)]
    fn get(&self, ns: Namespace) -> Option<&AttrMapEntry> {
        match ns {
            Namespace::Html => self.html.as_ref(),
            Namespace::Svg => self.svg.as_ref(),
        }
    }
}

pub struct AttrMap(HashMap<&'static [u8], ByNamespace>);

impl AttrMap {
    #[inline(always)]
    pub const fn new(map: HashMap<&'static [u8], ByNamespace>) -> AttrMap {
        AttrMap(map)
    }

    #[inline(always)]
    pub fn get(&self, ns: Namespace, tag: &[u8], attr: &[u8]) -> Option<&AttributeMinification> {
        self.0.get(attr).and_then(|namespaces| namespaces.get(ns)).and_then(|entry| match entry {
            AttrMapEntry::AllNamespaceElements(min) => Some(min),
            AttrMapEntry::SpecificNamespaceElements(map) => map.get(tag),
        })
    }
}

`;

  code += `
lazy_static! {
  pub static ref ATTRS: AttrMap = {
    let mut m = HashMap::<&'static [u8], ByNamespace>::new();
${[...attributes].map(([attr_name, namespaces]) => `    m.insert(b\"${attr_name}\", ByNamespace {
${(['html', 'svg'] as const).map(ns => `      ${ns}: ` + (() => {
        const tagsMap = namespaces.get(ns);
        if (!tagsMap) {
          return 'None';
        }
        const globalAttr = tagsMap.get('*');
        if (globalAttr) {
          return `Some(AttrMapEntry::AllNamespaceElements(${rsTagAttr(globalAttr)}))`;
        }
        return `Some({
        let mut m = HashMap::<&'static [u8], AttributeMinification>::new();
${[...tagsMap].map(([tagName, tagAttr]) => `        m.insert(b\"${tagName}\", ${rsTagAttr(tagAttr)});`).join('\n')}
        AttrMapEntry::SpecificNamespaceElements(m)
      })`;
      })() + ',').join('\n')}
    });

`).join('')}
    AttrMap::new(m)
  };
}`;
  return code;
};

const source = ts.createSourceFile(`react.d.ts`, reactDeclarations, ts.ScriptTarget.ES2020);
writeFileSync(join(RUST_OUT_DIR, 'attrs.rs'), processReactTypeDeclarations(source));
