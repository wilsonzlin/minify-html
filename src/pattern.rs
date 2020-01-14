use phf::{Map, Set};

pub struct SinglePattern {
    pub seq: &'static [u8],
    pub table: &'static [usize],
}

impl SinglePattern {
    pub fn match_against(&self, haystack: &[u8]) -> Option<usize> {
        let mut hay_idx = 0usize;
        let mut pat_idx = 0usize;
        while hay_idx < haystack.len() {
            if self.seq[pat_idx] == haystack[hay_idx] {
                pat_idx += 1;
                hay_idx += 1;
            };

            if pat_idx == self.seq.len() {
                return Some(hay_idx - pat_idx);
            };

            if hay_idx < haystack.len() && self.seq[pat_idx] != haystack[hay_idx] {
                if pat_idx != 0 {
                    pat_idx = self.table[pat_idx - 1];
                } else {
                    hay_idx += 1;
                };
            };
        };

        None
    }
}

pub enum AttrMapEntry {
    AllHtmlElements,
    SomeHtmlElements(&'static Set<&'static [u8]>),
}

pub struct AttrMap(Map<&'static [u8], AttrMapEntry>);

impl AttrMap {
    pub const fn new(map: Map<&'static [u8], AttrMapEntry>) -> AttrMap {
        AttrMap(map)
    }

    pub fn contains(&self, tag: &[u8], attr: &[u8]) -> bool {
        self.0.get(attr).filter(|elems| match elems {
            AttrMapEntry::AllHtmlElements => true,
            AttrMapEntry::SomeHtmlElements(set) => set.contains(tag),
        }).is_some()
    }
}
