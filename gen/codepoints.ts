// Official spec defined code points.
// See https://infra.spec.whatwg.org/#code-points for spec.

import {writeFileSync} from 'fs';
import {RUST_OUT_DIR} from './_common';
import {join} from 'path';

const rangeInclusive = (from: number, to: number) => Array.from({length: to - from + 1}, (_, i) => from + i);
const invert = (codepoints: number[]) => Array.from({length: 256}, (_, i) => codepoints.includes(i) ? undefined : i).filter(c => c != undefined);
const c = (char: string) => char.charCodeAt(0);

// Also update gen/tries.json when changing whitespace definition.
const WHITESPACE = [0x09, 0x0a, 0x0c, 0x0d, 0x20];
const C0_CONTROL = rangeInclusive(0, 0x1f);
const CONTROL = [...C0_CONTROL, ...rangeInclusive(0x7f, 0x9f)];
const DIGIT = rangeInclusive(c('0'), c('9'));
const UPPER_HEX_ALPHA = [...rangeInclusive(c('A'), c('F'))];
const LOWER_HEX_ALPHA = [...rangeInclusive(c('a'), c('f'))];
const HEX_DIGIT = [...DIGIT, ...UPPER_HEX_ALPHA, ...LOWER_HEX_ALPHA];
const UPPER_ALPHA = rangeInclusive(c('A'), c('Z'));
const LOWER_ALPHA = rangeInclusive(c('a'), c('z'));
const ALPHA = [...UPPER_ALPHA, ...LOWER_ALPHA];
const ALPHANUMERIC = [...DIGIT, ...ALPHA];

// Characters allowed in an attribute name.
// NOTE: Unicode noncharacters not tested.
// See https://html.spec.whatwg.org/multipage/syntax.html#syntax-attribute-name for spec.
const ATTR_NAME_CHAR = invert([...CONTROL, c(' '), c('"'), c('\''), c('>'), c('/'), c('=')]);

const DOUBLE_QUOTE = [c('"')];
const SINGLE_QUOTE = [c('\'')];
// Valid attribute quote characters.
// See https://html.spec.whatwg.org/multipage/introduction.html#intro-early-example for spec.
// Backtick is not a valid quote character according to spec.
const ATTR_QUOTE = [...DOUBLE_QUOTE, ...SINGLE_QUOTE];
// Valid unquoted attribute value characters.
// See https://html.spec.whatwg.org/multipage/syntax.html#unquoted for spec.
const NOT_UNQUOTED_ATTR_VAL_CHAR = [...WHITESPACE, c('"'), c('\''), c('='), c('<'), c('>'), c('`')];

// Tag names may only use ASCII alphanumerics. However, some people also use `:` and `-`.
// See https://html.spec.whatwg.org/multipage/syntax.html#syntax-tag-name for spec.
const TAG_NAME_CHAR = [...ALPHANUMERIC, c(':'), c('-')];

const output = `
pub struct Lookup {
  table: [bool; 256],
}

impl std::ops::Index<u8> for Lookup {
  type Output = bool;
  
  #[inline(always)]
  fn index(&self, c: u8) -> &Self::Output {
    // \`c\` is definitely below 256 so it's always safe to directly index table without checking.
    unsafe {
      self.table.get_unchecked(c as usize)
    } 
  }
}

` + Object.entries({
  WHITESPACE,
  DIGIT,
  UPPER_HEX_ALPHA,
  LOWER_HEX_ALPHA,
  HEX_DIGIT,

  ATTR_NAME_CHAR,

  DOUBLE_QUOTE,
  SINGLE_QUOTE,
  ATTR_QUOTE,
  NOT_UNQUOTED_ATTR_VAL_CHAR,

  TAG_NAME_CHAR,
})
  .map(([name, points]) => (`
pub static ${name}: &'static Lookup = &Lookup {
  table: [${
    Array.from({length: 256}, (_, i) => points.includes(i)).join(', ')
  }],
};`))
  .join('\n\n');

writeFileSync(join(RUST_OUT_DIR, 'codepoints.rs'), output);
