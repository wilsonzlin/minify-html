use crate::proc::Processor;
use crate::proc::uep::UnintentionalEntityState::*;
use crate::spec::codepoint::{is_digit, is_hex_digit};
use crate::unit::entity::{ENTITY_REFERENCES, is_valid_entity_reference_name_char};

macro_rules! uep_ignore {
    ($uep:ident, $proc:ident, $code:block) => {
        {
            $uep.suspend($proc);
            $code;
            $uep.resume($proc);
        }
    };
}

macro_rules! uep_process {
    ($uep:ident, $proc:ident, $code:block) => {
        {
            $uep.expect_active();
            $code;
            $uep.update($proc);
        }
    };
}

#[derive(Eq, PartialEq, Copy, Clone)]
enum UnintentionalEntityState {
    Suspended,
    Ended,
    Safe,
    Ampersand,
    Named,
    AmpersandHash,
    Dec,
    Hex,
    AfterEncodedRightChevron,
}

pub struct UnintentionalEntityPrevention {
    last_write_next: usize,
    ampersand_pos: usize,
    state: UnintentionalEntityState,
    encode_right_chevrons: bool,
}

impl UnintentionalEntityPrevention {
    pub fn expect_active(&self) -> () {
        debug_assert!(match self.state {
            Suspended | Ended => false,
            _ => true,
        });
    }

    pub fn new(proc: &Processor, encode_right_chevrons: bool) -> UnintentionalEntityPrevention {
        UnintentionalEntityPrevention {
            last_write_next: proc.write_next,
            ampersand_pos: 0,
            state: Safe,
            encode_right_chevrons,
        }
    }

    fn _handle_end_of_possible_entity(&mut self, proc: &mut Processor, end_inclusive: usize) -> usize {
        let should_encode_ampersand = match self.state {
            Named => match ENTITY_REFERENCES.longest_matching_prefix(&proc.code[self.ampersand_pos + 1..=end_inclusive]) {
                None => false,
                Some(_) => true,
            },
            Dec | Hex => true,
            _ => unreachable!(),
        };
        self.state = Safe;
        // Return added count rather than new absolute index as `end_inclusive` might not be `i` in `_after_write`.
        if should_encode_ampersand {
            // Insert encoded ampersand.
            proc._insert(self.ampersand_pos + 1, b"amp")
        } else {
            0
        }
    }

    fn _after_write(&mut self, proc: &mut Processor, is_end: bool) -> () {
        debug_assert!(self.state != Suspended);
        debug_assert!(self.state != Ended);
        debug_assert!(self.last_write_next <= proc.write_next);
        let mut i = self.last_write_next;
        // Use manual loop as `i` and `proc.write_next` could change due to mid-array insertion.
        while i < proc.write_next {
            let c = proc.code[i];
            if c == b'>' && self.encode_right_chevrons {
                if self.state == Named {
                    i += self._handle_end_of_possible_entity(proc, i - 1);
                };
                self.state = AfterEncodedRightChevron;
                // Use "&GT" instead of "&gt" as there are other entity names starting with "gt".
                i += proc._replace(i, i + 1, b"&GT");
            } else {
                match (self.state, c) {
                    // Problem: semicolon after encoded '>' will cause '&GT;', making it part of the entity.
                    // Solution: insert another semicolon.
                    (AfterEncodedRightChevron, b';') => {
                        self.state = Safe;
                        i += proc._insert(i, b";");
                    }

                    // If ampersand, then regardless of state, this is the start of a new entity.
                    (s, b'&') => {
                        if s == Named {
                            i += self._handle_end_of_possible_entity(proc, i - 1);
                        };
                        self.state = Ampersand;
                        self.ampersand_pos = i;
                    }

                    (Ampersand, b'#') => self.state = AmpersandHash,
                    (Ampersand, c) if is_valid_entity_reference_name_char(c) => self.state = Named,

                    (AmpersandHash, b'x') => self.state = Hex,
                    (AmpersandHash, c) if is_digit(c) => {
                        self.state = Dec;
                        i += self._handle_end_of_possible_entity(proc, i);
                    }

                    // TODO Maybe should limit count?
                    // NOTE: Cannot try to match trie right now as we need to find longest match.
                    (Named, c) if is_valid_entity_reference_name_char(c) => {}
                    (Named, b';') => i += self._handle_end_of_possible_entity(proc, i),
                    (Named, _) => i += self._handle_end_of_possible_entity(proc, i - 1),

                    (Hex, c) if is_hex_digit(c) => i += self._handle_end_of_possible_entity(proc, i),

                    (Safe, _) => {}
                    (AfterEncodedRightChevron, _) | (Ampersand, _) | (AmpersandHash, _) | (Hex, _) => self.state = Safe,
                    // Dec state is unreachable.
                    _ => unreachable!(),
                };
            };
            i += 1;
        };
        if is_end && self.state == Named {
            self._handle_end_of_possible_entity(proc, proc.write_next - 1);
        };
        self.last_write_next = proc.write_next;
    }

    pub fn update(&mut self, proc: &mut Processor) -> () {
        self._after_write(proc, false);
    }

    pub fn end(&mut self, proc: &mut Processor) -> () {
        self._after_write(proc, true);
        self.state = Ended;
    }

    pub fn suspend(&mut self, proc: &mut Processor) -> () {
        if self.state != Suspended {
            self._after_write(proc, true);
            self.state = Suspended;
        };
    }

    pub fn resume(&mut self, proc: &Processor) -> () {
        debug_assert!(self.state == Suspended);
        self.last_write_next = proc.write_next;
        self.state = Safe;
    }
}
