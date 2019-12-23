// Official spec defined code points.
// See https://infra.spec.whatwg.org/#code-points for spec.

pub fn is_tab_or_newline(c: u8) -> bool {
    match c {
        0x09 | 0x0a | 0x0d => true,
        _ => false,
    }
}

pub fn is_whitespace(c: u8) -> bool {
    // Also update crate::proc::attr::quoted::STATIC when changing here.
    match c {
        0x09 | 0x0a | 0x0c | 0x0d | 0x20 => true,
        _ => false,
    }
}

pub fn is_c0_control(c: u8) -> bool {
    c >= 0 && c <= 0x1f
}

pub fn is_control(c: u8) -> bool {
    is_c0_control(c) || c >= 0x7f && c <= 0x9f
}

pub fn is_digit(c: u8) -> bool {
    c >= b'0' && c <= b'9'
}

pub fn is_upper_hex_digit(c: u8) -> bool {
    is_digit(c) || c >= b'A' && c <= b'F'
}

pub fn is_lower_hex_digit(c: u8) -> bool {
    is_digit(c) || c >= b'a' && c <= b'f'
}

pub fn is_hex_digit(c: u8) -> bool {
    is_upper_hex_digit(c) || is_lower_hex_digit(c)
}

pub fn is_upper_alpha(c: u8) -> bool {
    c >= b'A' && c <= b'Z'
}

pub fn is_lower_alpha(c: u8) -> bool {
    c >= b'a' && c <= b'z'
}

pub fn is_alpha(c: u8) -> bool {
    is_upper_alpha(c) || is_lower_alpha(c)
}

pub fn is_alphanumeric(c: u8) -> bool {
    is_digit(c) || is_alpha(c)
}
