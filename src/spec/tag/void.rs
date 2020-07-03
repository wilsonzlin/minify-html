use lazy_static::lazy_static;
use std::collections::HashSet;

lazy_static! {
    pub static ref VOID_TAGS: HashSet<&'static [u8]> = {
        let mut s = HashSet::<&'static [u8]>::new();
        s.insert(b"area");
        s.insert(b"base");
        s.insert(b"br");
        s.insert(b"col");
        s.insert(b"embed");
        s.insert(b"hr");
        s.insert(b"img");
        s.insert(b"input");
        s.insert(b"keygen");
        s.insert(b"link");
        s.insert(b"meta");
        s.insert(b"param");
        s.insert(b"source");
        s.insert(b"track");
        s.insert(b"wbr");
        s
    };
}
