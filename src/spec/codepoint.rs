// Official spec defined code points.
// See https://infra.spec.whatwg.org/#code-points for spec.

// Also update gen/tries.json when changing here.
// 0x09 | 0x0a | 0x0c | 0x0d | 0x20.
static IS_WHITESPACE: [bool; 256] = [
    false, false, false, false, false, false, false, false, false, true, true, false, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
];

#[inline(always)]
pub fn is_whitespace(c: u8) -> bool {
    IS_WHITESPACE[c as usize]
}

#[inline(always)]
pub fn is_c0_control(c: u8) -> bool {
    // c >= 0 is always true.
    c <= 0x1f
}

#[inline(always)]
pub fn is_control(c: u8) -> bool {
    is_c0_control(c) || c >= 0x7f && c <= 0x9f
}

#[inline(always)]
pub fn is_digit(c: u8) -> bool {
    c >= b'0' && c <= b'9'
}

#[inline(always)]
pub fn is_upper_hex_digit(c: u8) -> bool {
    is_digit(c) || c >= b'A' && c <= b'F'
}

#[inline(always)]
pub fn is_lower_hex_digit(c: u8) -> bool {
    is_digit(c) || c >= b'a' && c <= b'f'
}

#[inline(always)]
pub fn is_hex_digit(c: u8) -> bool {
    is_upper_hex_digit(c) || is_lower_hex_digit(c)
}

#[inline(always)]
pub fn is_upper_alpha(c: u8) -> bool {
    c >= b'A' && c <= b'Z'
}

#[inline(always)]
pub fn is_lower_alpha(c: u8) -> bool {
    c >= b'a' && c <= b'z'
}

#[inline(always)]
pub fn is_alpha(c: u8) -> bool {
    is_upper_alpha(c) || is_lower_alpha(c)
}

#[inline(always)]
pub fn is_alphanumeric(c: u8) -> bool {
    is_digit(c) || is_alpha(c)
}
