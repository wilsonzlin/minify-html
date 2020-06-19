import {readFileSync, writeFileSync} from 'fs';
import {join} from 'path';
import {byteStringLiteral, DATA_DIR, RUST_OUT_DIR} from './_common';
import {TrieBuilder} from './trie';

const entities: {[name: string]: {codepoints: number[]; characters: string;}} = JSON.parse(readFileSync(join(DATA_DIR, 'entities.json'), 'utf8'));

const trieBuilder = new TrieBuilder('ENTITY', "&'static [u8]");
for (const [rep, entity] of Object.entries(entities)) {
  const bytes = Buffer.from(entity.characters, 'utf8');
  // Since we're minifying in place, we need to guarantee we'll never write something longer than source.
  const val = byteStringLiteral(rep.length < bytes.length ? [...rep].map(c => c.charCodeAt(0)) : [...bytes]);
  trieBuilder.add(rep.slice(1), val);
}

const output = `
${trieBuilder.generate()}
`;
writeFileSync(join(RUST_OUT_DIR, 'entities.rs'), output);
