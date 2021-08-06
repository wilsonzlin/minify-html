import {readFileSync, writeFileSync} from 'fs';
import {join} from 'path';
import {byteStringLiteral, DATA_DIR, RUST_OUT_DIR} from './_common';
import {parsePattern, TrieBuilder} from './trie';

const entities: {[name: string]: {codepoints: number[]; characters: string;}} = JSON.parse(readFileSync(join(DATA_DIR, 'entities.json'), 'utf8'));

const trieBuilder = new TrieBuilder('ENTITY', "EntityType");
trieBuilder.addPattern(parsePattern("&#[0-9]"), 'EntityType::Dec');
trieBuilder.addPattern(parsePattern("&#x[0-9a-fA-F]"), 'EntityType::Hex');
for (const [encoded, entity] of Object.entries(entities)) {
  const encodedBytes = Buffer.from(encoded, "utf8");
  const decodedBytes = Buffer.from(entity.characters, 'utf8');
  // We should not decode if encoded is shorter than decoded.
  const val = byteStringLiteral([...encodedBytes.length < decodedBytes.length ? encodedBytes : decodedBytes]);
  trieBuilder.add(encoded, `EntityType::Named(${val})`);
}

const output = `
#[derive(Clone, Copy)]
pub enum EntityType {
  Named(&'static [u8]),
  Dec,
  Hex,
}

${trieBuilder.generate()}
`;
writeFileSync(join(RUST_OUT_DIR, 'entities.rs'), output);
