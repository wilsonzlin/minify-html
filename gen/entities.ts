import { readFileSync, writeFileSync } from "fs";
import { join } from "path";
import { byteStringLiteral, DATA_DIR, RUST_OUT_DIR } from "./_common";
import { parsePattern, TrieBuilder } from "./trie";

const entities: {
  [name: string]: { codepoints: number[]; characters: string };
} = JSON.parse(readFileSync(join(DATA_DIR, "entities.json"), "utf8"));

const trieBuilder = new TrieBuilder("ENTITY", "EntityType");
trieBuilder.addPattern(parsePattern("&#[0-9]"), "EntityType::Dec");
trieBuilder.addPattern(parsePattern("&#x[0-9a-fA-F]"), "EntityType::Hex");
const shorterEncodedEntities = [];
for (const [encoded, entity] of Object.entries(entities)) {
  const encodedBytes = Buffer.from(encoded, "utf8");
  const decodedBytes = Buffer.from(entity.characters, "utf8");
  const val = byteStringLiteral([...decodedBytes]);
  trieBuilder.add(encoded, `EntityType::Named(${val})`);
  // We should encode if encoded is shorter than decoded.
  if (encodedBytes.byteLength < decodedBytes.byteLength) {
    shorterEncodedEntities.push([
      byteStringLiteral([...encodedBytes]),
      val,
    ] as const);
  }
}

const output = `
pub static SHORTER_ENCODED_ENTITIES_ENCODED: &[&'static [u8]] = &[
  ${shorterEncodedEntities.map(([encoded, _]) => encoded).join(",\n  ")}
];
pub static SHORTER_ENCODED_ENTITIES_DECODED: &[&'static [u8]] = &[
  ${shorterEncodedEntities.map(([_, decoded]) => decoded).join(",\n  ")}
];

#[derive(Clone, Copy)]
pub enum EntityType {
  Named(&'static [u8]),
  Dec,
  Hex,
}

${trieBuilder.generate()}
`;
writeFileSync(join(RUST_OUT_DIR, "entities.rs"), output);
