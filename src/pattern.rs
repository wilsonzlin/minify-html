pub struct SinglePattern {
    dfa: &'static [usize],
    length: usize,
}

impl SinglePattern {
    pub const fn prebuilt(dfa: &'static [usize], length: usize) -> SinglePattern {
        SinglePattern {
            dfa,
            length,
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
    // Using a children array of size 256 would probably be fastest, but waste too much memory and cause slow compiles
    // and large binaries. Instead, we only store the children between the first and last defined (see `gen/trie.ts`).
    // When getting a child, use `index + offset`.
    pub offset: usize,
    pub value: Option<V>,
    pub children: &'static [Option<&'static TrieNode<V>>],
}

pub enum TrieNodeMatch<V: 'static + Copy> {
    Found { len: usize, value: V },
    NotFound { reached: usize },
}

impl<V: 'static + Copy> TrieNodeMatch<V> {
    pub fn found(&self) -> bool {
        match self {
            TrieNodeMatch::Found { .. } => true,
            TrieNodeMatch::NotFound { .. } => false,
        }
    }
}

impl<V: 'static + Copy> TrieNode<V> {
    // Find the node that matches the shortest prefix of {@param text} and has a value, or the entire text.
    #[inline(always)]
    pub fn next_matching_node(&self, text: &[u8], from: usize) -> Option<(&TrieNode<V>, usize)> {
        let mut node: &TrieNode<V> = self;
        let mut next_pos = from;
        while let Some(&c) = text.get(next_pos) {
            match node.children.get(c as usize + node.offset) {
                Some(Some(child)) => node = child,
                None | Some(None) => return None,
            };
            next_pos += 1;
            if node.value.is_some() {
                break;
            };
        };
        Some((node, next_pos))
    }

    #[inline(always)]
    pub fn longest_matching_prefix(&self, text: &[u8]) -> TrieNodeMatch<V> {
        let mut node: &TrieNode<V> = self;
        let mut value: Option<TrieNodeMatch<V>> = None;
        let mut pos = 0;
        while let Some((new_node, new_pos)) = node.next_matching_node(text, pos) {
            value = Some(TrieNodeMatch::Found { len: pos, value: new_node.value.unwrap() });
            node = new_node;
            pos = new_pos;
        };
        value.unwrap_or(TrieNodeMatch::NotFound { reached: pos })
    }
}
