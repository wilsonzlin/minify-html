import {readFileSync, writeFileSync} from 'fs';
import {DATA_DIR, RUST_OUT_DIR} from './_common';
import {join} from 'path';
import {EOL} from 'os';

const patterns: {[name: string]: string} = JSON.parse(readFileSync(join(DATA_DIR, 'patterns.json'), 'utf8'));

const chr = (str: string, char: number) => str.charCodeAt(char);

const buildPattern = (seq: string): string => {
  const dfa = Array.from({length: 256}, () => Array(seq.length).fill(0));

  dfa[chr(seq, 0)][0] = 1;
  let  x = 0;
  let  j = 1;
  while (j < seq.length) {
    for (let c = 0; c < 256; c++) {
      dfa[c][j] = dfa[c][x];
    }
    dfa[chr(seq, j)][j] = j + 1;
    x = dfa[chr(seq, j)][x];
    j += 1;
  }

  return `crate::pattern::SinglePattern::prebuilt(&[${dfa.flat().join(', ')}], ${seq.length})`;
};

const output = Object.entries(patterns)
  .map(([name, pattern]) => `pub static ${name}: &crate::pattern::SinglePattern = &${buildPattern(pattern)};`);

writeFileSync(join(RUST_OUT_DIR, 'patterns.rs'), output.join(EOL));
