use crate::common::gen::codepoints::WHITESPACE;

pub fn trimmed(val: &[u8]) -> &[u8] {
    let mut start = 0;
    while val.get(start).filter(|&&c| WHITESPACE[c]).is_some() {
        start += 1;
    }

    let mut end = val.len();
    while end > start && val.get(end - 1).filter(|&&c| WHITESPACE[c]).is_some() {
        end -= 1;
    }

    &val[start..end]
}

pub fn left_trim(val: &mut Vec<u8>) {
    let mut len = 0;
    while val.get(len).filter(|&&c| WHITESPACE[c]).is_some() {
        len += 1;
    }
    val.drain(0..len);
}

pub fn right_trim(val: &mut Vec<u8>) {
    let mut retain = val.len();
    while retain > 0 && val.get(retain - 1).filter(|&&c| WHITESPACE[c]).is_some() {
        retain -= 1;
    }
    val.truncate(retain);
}

pub fn collapse_whitespace(val: &mut Vec<u8>) {
    let mut write = 0;
    let mut in_whitespace = false;
    for i in 0..val.len() {
        let mut c = val[i];
        if WHITESPACE[c] {
            if in_whitespace {
                // Skip this character.
                continue;
            };
            in_whitespace = true;
            c = b' ';
        } else {
            in_whitespace = false;
        };
        val[write] = c;
        write += 1;
    }
    val.truncate(write);
}

pub fn remove_all_whitespace(val: &mut Vec<u8>) {
    let mut write = 0;
    for i in 0..val.len() {
        let c = val[i];
        if WHITESPACE[c] {
            // Skip this character.
            continue;
        };
        val[write] = c;
        write += 1;
    }
    val.truncate(write);
}

pub fn is_all_whitespace(val: &[u8]) -> bool {
    for &c in val {
        if !WHITESPACE[c] {
            return false;
        };
    }
    true
}
