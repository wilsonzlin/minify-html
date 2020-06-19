import yaml from 'yaml';
import {DATA_DIR, RUST_OUT_DIR} from './_common';
import {readFileSync, writeFileSync} from 'fs';
import {join} from 'path';
import {EOL} from 'os';
import {parsePattern, TrieBuilder} from './trie';

const dfa: { [node: string]: { [transition: string]: string } } = yaml.parse(readFileSync(join(DATA_DIR, 'dfa.yaml'), 'utf8'));
// These states must always exist; see lex/mod.rs for more details.
dfa['TextEntity'] = {};
dfa['AttrValueEntity'] = {};
dfa['Unknown'] = {};
dfa['EOF'] = {};

const nodes = Object.keys(dfa).sort();

const rsTransition = (val: string) => {
  const [_, flag, next] = /^([_<+?]?)(.*)$/.exec(val)!;
  const consumeMode = {
    '_': 'AccumulateLowerCase',
    '': 'Accumulate',
    '<': 'Current',
    '+': 'Next',
    '?': 'Reconsume',
  }[flag];
  return `Transition {
    to: State::${next},
    consume: ConsumeMode::${consumeMode},
  }`;
};

const output = `
#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub enum State {
  ${nodes.map((n, i) => `${n} = ${i}`).join(`,${EOL}  `)}
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub enum ConsumeMode {
  Current,
  Next,
  Reconsume,
  Accumulate,
  AccumulateLowerCase,
}

#[derive(Clone, Copy)]
pub struct Transition {
  // Make pub to allow destructuring.
  pub to: State,
  pub consume: ConsumeMode,
}

${nodes.map(n => {
  const trieBuilder = new TrieBuilder(n.toUpperCase(), 'Transition');
  for (const [pat, val] of Object.entries(dfa[n])) {
    if (pat == '') {
      continue;
    }
    trieBuilder.addPattern(parsePattern(pat), rsTransition(val));
  }
  if (dfa[n][''] !== undefined) {
    trieBuilder.fillRemaining(rsTransition(dfa[n]['']));
  }
  return trieBuilder.generate();
}).join(EOL + EOL)}

pub static TRANSITIONS: [&'static crate::pattern::TrieNode<Transition>; ${nodes.length}] = [${nodes.map(n => n.toUpperCase()).join(', ')}];
`;

writeFileSync(join(RUST_OUT_DIR, 'dfa.rs'), output);
