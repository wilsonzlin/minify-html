pub struct SinglePattern {
    seq: &'static [u8],
    table: Vec<usize>,
}

impl SinglePattern {
    pub fn new(seq: &'static [u8]) -> SinglePattern {
        let mut max_prefix_len = 0usize;
        let mut table = vec![0usize; seq.len()];

        let mut i = 1;
        while i < seq.len() {
            if seq[i] == seq[max_prefix_len] {
                max_prefix_len += 1;
                table[i] = max_prefix_len;
                i += 1;
            } else {
                if max_prefix_len != 0 {
                    max_prefix_len = table[max_prefix_len - 1];
                } else {
                    table[i] = 0;
                    i += 1;
                };
            };
        };

        SinglePattern {
            seq,
            table,
        }
    }

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