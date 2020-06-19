import {EOL} from 'os';

const customCharClasses = {
  tagName: '[a-zA-Z-]',
  attrName: '[a-zA-Z-]',
};

const whitespaceClass = [' ', '\r', '\n', '\t', '\v', '\f'];

const charRange = (from: string, to: string) => {
  const res = [];
  for (let i = from.charCodeAt(0); i <= to.charCodeAt(0); i++) {
    res.push(String.fromCharCode(i));
  }
  return res;
};

const parsePatternEscape = (pat: string, at: number): string[] => {
  switch (pat[at]) {
  case '\\':
    return ['\\'];
  case ']':
    return [']'];
  case '<':
    return ['<'];
  case 'w':
    return whitespaceClass;
  default:
    throw new Error(`Unknown pattern escape: ${pat[at]}`);
  }
};

const parsePatternClass = (pat: string, from: number): [string[], number] => {
  const chars: string[] = [];
  for (let i = from; i < pat.length; i++) {
    switch (pat[i]) {
    case '\\':
      chars.push(...parsePatternEscape(pat, ++i));
      break;
    case ']':
      return [chars, i];
    default:
      if (pat[i + 1] === '-' && pat[i + 2] !== undefined) {
        chars.push(...charRange(pat[i], pat[i + 2]));
        i += 2;
      } else {
        chars.push(pat[i]);
      }
      break;
    }
  }
  throw new Error(`Unexpected end of pattern: ${pat}`);
};

const parsePatternCustomClass = (pat: string, from: number): [string[], number] => {
  const endIdx = pat.indexOf('>', from);
  if (endIdx == -1) throw new Error(`Unexpected end of pattern: ${pat}`);
  return [parsePatternClass(customCharClasses[pat.slice(from, endIdx)], 1)[0], endIdx];
};

export const parsePattern = (pat: string): string[][] => {
  const res: string[][] = [];
  for (let i = 0; i < pat.length; i++) {
    switch (pat[i]) {
    case '\\':
      res.push(parsePatternEscape(pat, ++i));
      break;
    case '[':
      const sg = parsePatternClass(pat, i + 1);
      res.push(sg[0]);
      i = sg[1];
      break;
    case '<':
      const cc = parsePatternCustomClass(pat, i + 1);
      res.push(cc[0]);
      i = cc[1];
      break;
    default:
      res.push([pat[i]]);
    }
  }
  return res;
};

type Node = {
  children: Node[];
  value?: string;
};

const createNode = (value?: string) => ({value, children: []});

export class TrieBuilder {
  private readonly root: Node = createNode();

  private readonly variables: string[] = [];
  private nextId: number = 0;
  private readonly codeCache: Map<string, string> = new Map();

  constructor (
    private readonly name: string,
    private readonly valueType: string,
  ) {
  }

  fillRemaining (val: string): this {
    const {children} = this.root;
    for (let i = 0; i < 256; i++) {
      children[i] = children[i] || createNode(val);
    }
    return this;
  }

  add (seq: string, val: string): this {
    let cur: Node = this.root;
    for (let i = 0; i < seq.length; i++) {
      const c = seq.charCodeAt(i);
      if (c > 255) throw new Error('Not a byte');
      cur = cur.children[c] = cur.children[c] || createNode();
    }
    cur.value = val;
    return this;
  }

  addPattern (pattern: string[][], val: string): this {
    let cur: Node[] = [this.root];
    for (const cls of pattern) {
      const next: Node[] = [];
      for (let i = 0; i < cls.length; i++) {
        if (cls[i].length !== 1) throw new Error(`Not a byte`);
        const c = cls[i].charCodeAt(0);
        if (c > 255) throw new Error('Not a byte');
        next.push(...cur.map(n => n.children[c] = n.children[c] || createNode()));
      }
      cur = next;
    }
    cur.forEach(n => n.value = val);
    return this;
  }

  // Generate the code for a node's variable name and value, and return the name.
  private generateNode (node: Node): string {
    // Only generate defined children to cut down on size of array, which would otherwise
    // bog down compile time and binary size for large trees with lots of nodes.
    // If array is empty, just use zero.
    const firstIdx = node.children.length && node.children.findIndex(v => v);
    const children = Array.from(
      {length: node.children.length - firstIdx},
      (_, i) => node.children[i + firstIdx] ? `Some(${this.generateNode(node.children[i + firstIdx])})` : 'None',
    ).join(', ');

    const value = node.value === undefined ? 'None' : `Some(${node.value})`;
    const varValue = `&crate::pattern::TrieNode {
      offset: ${firstIdx},
      value: ${value},
      children: &[${children}],
    }`;
    const existingVarName = this.codeCache.get(varValue);
    if (existingVarName) {
      return existingVarName;
    }

    const name = `${this.name}_NODE_${this.nextId++}`;
    this.variables.push(`static ${name}: &'static crate::pattern::TrieNode<${this.valueType}> = ${varValue};`);
    this.codeCache.set(varValue, name);
    return name;
  }

  generate (): string {
    this.variables.splice(0, this.variables.length);
    this.nextId = 0;
    const rootName = this.generateNode(this.root);
    // Make root node public and use proper name.
    return this.variables.join(EOL + EOL).replace(`static ${rootName}`, `pub static ${this.name}`);
  }
}
