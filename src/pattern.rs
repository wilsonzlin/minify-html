// Can't use pub const fn constructor due to Copy trait, so allow directly creating struct publicly for now.
pub struct TrieNode<V: 'static + Copy> {
    // Using a children array of size 256 would probably be fastest, but waste too much memory and cause slow compiles
    // and large binaries. Instead, we only store the children between the first and last defined (see `gen/trie.ts`).
    // When getting a child, use `index - offset`.
    pub offset: usize,
    pub value: Option<V>,
    pub children: &'static [Option<&'static TrieNode<V>>],
}

pub enum TrieNodeMatch<V: 'static + Copy> {
    Found { len: usize, value: V },
    NotFound { reached: usize },
}

impl<V: 'static + Copy> TrieNode<V> {
    // Find the node that matches the shortest prefix of {@param text} that:
    // - has a value (except the start node if it has a value);
    // - fails to match any further characters (the node itself matches); or,
    // - the entire text (essentially same as previous point).
    //
    // For example, given a trie with only two paths "&amp" and "&amp;":
    // - "&amp" will return node `p`.
    // - "&ampere" will return node `p`.
    // - "&amp;" will return node `p`.
    // - "&amp;ere" will return node `p`.
    // - "&am" will return node `m`.
    //   - Further matching "p;" will return node `p`.
    //   - Further matching "xyz" will return node `m` (itself).
    // - "&amx" will return node `m`.
    // - "&ax" will return node `a`.
    // - "+ax" will return itself.
    // - "" will return the itself.
    #[inline(always)]
    pub fn shortest_matching_prefix(&self, text: &[u8], from: usize) -> (&TrieNode<V>, usize) {
        let mut node: &TrieNode<V> = self;
        let mut pos = from;
        while let Some(&c) = text.get(pos) {
            match node.children.get((c as usize).wrapping_sub(node.offset)) {
                Some(Some(child)) => node = child,
                None | Some(None) => break,
            };
            pos += 1;
            if node.value.is_some() {
                break;
            };
        };
        (node, pos)
    }

    #[inline(always)]
    pub fn longest_matching_prefix(&self, text: &[u8]) -> TrieNodeMatch<V> {
        let mut node: &TrieNode<V> = self;
        let mut value: Option<TrieNodeMatch<V>> = None;
        let mut pos = 0;
        while let Some(&c) = text.get(pos) {
            match node.children.get((c as usize).wrapping_sub(node.offset)) {
                Some(Some(child)) => node = child,
                None | Some(None) => break,
            };
            pos += 1;
            match node.value {
                Some(v) => value = Some(TrieNodeMatch::Found { len: pos, value: v }),
                None => {}
            };
        };
        value.unwrap_or(TrieNodeMatch::NotFound { reached: pos })
    }
}
