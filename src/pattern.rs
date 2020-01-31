pub struct SinglePattern {
    dfa: &'static [usize],
    length: usize,
}

impl SinglePattern {
    pub const fn prebuilt(dfa: &'static [usize], length: usize) -> SinglePattern {
        SinglePattern {
            dfa, length
        }
    }

    pub fn len(&self) -> usize {
        self.length
    }

    #[inline(always)]
    pub fn match_against(&self, haystack: &[u8]) -> Option<usize> {
        let mut i = 0;
        let mut j = 0;
        while i < haystack.len() && j < self.length {
            j = self.dfa[haystack[i] as usize * self.length + j];
            i += 1;
        };
        if j == self.length {
            Some(i - self.length)
        } else {
            None
        }
    }
}

// Can't use pub const fn constructor due to Copy trait, so allow directly creating struct publicly for now.
pub struct TrieNode<V: 'static + Copy> {
    pub value: Option<V>,
    pub children: [Option<&'static TrieNode<V>>; 256],
}

pub struct TrieNodeMatch<V: 'static + Copy> {
    pub end: usize,
    pub value: V,
}

impl<V: 'static + Copy> TrieNode<V> {
    #[inline(always)]
    pub fn longest_matching_prefix(&self, text: &[u8]) -> Option<TrieNodeMatch<V>> {
        let mut node: &TrieNode<V> = self;
        let mut value: Option<TrieNodeMatch<V>> = None;
        for (i, &c) in text.iter().enumerate() {
            match node.children[c as usize] {
                Some(child) => node = child,
                None => break,
            };
            match node.value {
                Some(v) => value = Some(TrieNodeMatch { end: i, value: v }),
                None => {}
            };
        };
        value
    }
}

