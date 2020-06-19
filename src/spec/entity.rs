// Based on the data sourced from https://html.spec.whatwg.org/entities.json:
// - Entity names can have [A-Za-z0-9] characters, and are case sensitive.
// - Some character entity references do not end with a semicolon.
//   - All of these entities also have a corresponding entity with semicolon.
// - The longest name is "CounterClockwiseContourIntegral", with length 31
// (excluding leading ampersand and trailing semicolon).
// - All entity names are at least 2 characters long.
// - Some named entities are actually shorter than their decoded characters as UTF-8.

// Browser implementation behaviour to consider:
// - Browsers match longest sequence of characters that would form a valid entity.
// - Names must match case sensitively.
// - For a numeric entity, browsers actually consume an unlimited amount of digits, but decode to 0xFFFD if not a valid
//   Unicode Scalar Value.

pub fn is_entity_reference_name_char(c: u8) -> bool {
    c >= b'0' && c <= b'9' || c >= b'A' && c <= b'Z' || c >= b'a' && c <= b'z'
}
