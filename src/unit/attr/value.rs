use phf::{Map, phf_map};

use crate::err::ProcessingResult;
use crate::proc::{Processor, ProcessorRange};
use crate::spec::codepoint::is_whitespace;
use crate::unit::entity::{EntityType, maybe_process_entity, ParsedEntity};

pub fn is_double_quote(c: u8) -> bool {
    c == b'"'
}

pub fn is_single_quote(c: u8) -> bool {
    c == b'\''
}

// Valid attribute quote characters.
// See https://html.spec.whatwg.org/multipage/introduction.html#intro-early-example for spec.
pub fn is_attr_quote(c: u8) -> bool {
    // Backtick is not a valid quote character according to spec.
    is_double_quote(c) || is_single_quote(c)
}

pub fn is_unquoted_delimiter(c: u8) -> bool {
    is_whitespace(c) || c == b'>'
}

static ENCODED: Map<u8, &'static [u8]> = phf_map! {
    b'\'' => b"&#39;",
    b'"' => b"&#34;",
    b'>' => b"&gt;",
    // Whitespace characters as defined by spec in crate::spec::codepoint::is_whitespace.
    b'\x09' => b"&#9;",
    b'\x0a' => b"&#10;",
    b'\x0c' => b"&#12;",
    b'\x0d' => b"&#13;",
    b'\x20' => b"&#32;",
};

#[derive(Clone, Copy, Eq, PartialEq)]
enum CharType {
    End,
    NonAsciiEntity(ParsedEntity),
    // Normal needs associated character to be able to write it.
    Normal(u8),
    // Whitespace needs associated character to determine cost of encoding it.
    Whitespace(u8),
    SingleQuote,
    DoubleQuote,
    RightChevron,
}

impl CharType {
    fn from_char(c: u8) -> CharType {
        match c {
            b'"' => CharType::DoubleQuote,
            b'\'' => CharType::SingleQuote,
            b'>' => CharType::RightChevron,
            c => if is_whitespace(c) { CharType::Whitespace(c) } else { CharType::Normal(c) },
        }
    }
}

#[derive(Clone, Copy, Eq, PartialEq)]
pub enum DelimiterType {
    Double,
    Single,
    Unquoted,
}

struct Metrics {
    count_double_quotation: usize,
    count_single_quotation: usize,
    // NOTE: This count is amount after any trimming and collapsing of whitespace.
    count_whitespace: usize,
    // Since whitespace characters have varying encoded lengths, also calculate total length if all of them had to be encoded.
    total_whitespace_encoded_length: usize,
    // First and last character value types after any trimming and collapsing of whitespace.
    // NOTE: First/last value characters, not quotes/delimiters.
    first_char_type: Option<CharType>,
    last_char_type: Option<CharType>,
    // How many times `collect_char_type` is called. Used to determine first and last characters when writing.
    // NOTE: This may not be the same as amount of final characters, as malformed entities are usually multiple chars.
    collected_count: usize,
}

impl Metrics {
    // Update metrics with next character type.
    fn collect_char_type(&mut self, char_type: CharType) -> () {
        match char_type {
            CharType::Whitespace(c) => {
                self.count_whitespace += 1;
                self.total_whitespace_encoded_length += ENCODED[&c].len();
            }
            CharType::SingleQuote => self.count_single_quotation += 1,
            CharType::DoubleQuote => self.count_double_quotation += 1,
            _ => (),
        };

        if let None = self.first_char_type {
            self.first_char_type = Some(char_type);
        };
        self.last_char_type = Some(char_type);
        self.collected_count += 1;
    }

    fn unquoted_cost(&self) -> usize {
        // Costs for encoding first and last characters if going with unquoted attribute value.
        // NOTE: Don't need to consider whitespace for either as all whitespace will be encoded and counts as part of `total_whitespace_encoded_length`.
        let first_char_encoding_cost = match self.first_char_type {
            // WARNING: Change `first_char_is_quote_encoded` if changing here.
            Some(CharType::DoubleQuote) => ENCODED[&b'"'].len(),
            Some(CharType::SingleQuote) => ENCODED[&b'\''].len(),
            _ => 0,
        };
        let first_char_is_quote_encoded = first_char_encoding_cost > 0;
        let last_char_encoding_cost = match self.last_char_type {
            Some(CharType::RightChevron) => ENCODED[&b'>'].len(),
            _ => 0,
        };

        self.count_single_quotation
            + self.count_double_quotation
            + self.total_whitespace_encoded_length
            + first_char_encoding_cost
            + last_char_encoding_cost
            // If first char is quote and is encoded, it will be counted twice as it'll also be part of `metrics.count_*_quotation`.
            // Subtract last to prevent underflow.
            - first_char_is_quote_encoded as usize
    }

    fn single_quoted_cost(&self) -> usize {
        self.count_single_quotation * ENCODED[&b'\''].len()
            + self.count_double_quotation
            + self.count_whitespace
            + 2 // Delimiter quotes.
    }

    fn double_quoted_cost(&self) -> usize {
        self.count_single_quotation
            + self.count_double_quotation * ENCODED[&b'"'].len()
            + self.count_whitespace
            + 2 // Delimiter quotes.
    }

    fn get_optimal_delimiter_type(&self) -> DelimiterType {
        // When all equal, prefer double quotes to all and single quotes to unquoted.
        let mut min = (DelimiterType::Double, self.double_quoted_cost());

        let single = (DelimiterType::Single, self.single_quoted_cost());
        if single.1 < min.1 {
            min = single;
        };

        let unquoted = (DelimiterType::Unquoted, self.unquoted_cost());
        if unquoted.1 < min.1 {
            min = unquoted;
        };

        min.0
    }
}

macro_rules! consume_attr_value_chars {
    ($proc:ident, $should_collapse_and_trim_ws:ident, $delimiter_pred:ident, $out_char_type:ident, $on_char:block) => {
        // Set to true when one or more immediately previous characters were whitespace and deferred for processing after the contiguous whitespace.
        // NOTE: Only used if `should_collapse_and_trim_ws`.
        let mut currently_in_whitespace = false;
        // Needed to check if at beginning of value so that leading whitespace can be trimmed instead of collapsed.
        // NOTE: Only used if `should_collapse_and_trim_ws`.
        let mut currently_first_char = true;

        loop {
            let char_type = if chain!($proc.match_pred($delimiter_pred).matched()) {
                // DO NOT BREAK HERE. More processing is done afterwards upon reaching end.
                CharType::End
            } else if chain!($proc.match_char(b'&').matched()) {
                let entity = maybe_process_entity($proc)?;
                if let EntityType::Ascii(c) = entity.entity() {
                    CharType::from_char(c)
                } else {
                    CharType::NonAsciiEntity(entity)
                }
            } else {
                CharType::from_char($proc.skip()?)
            };

            if $should_collapse_and_trim_ws {
                if let CharType::Whitespace(_) = char_type {
                    // Ignore this whitespace character, but mark the fact that we are currently in contiguous whitespace.
                    currently_in_whitespace = true;
                    continue;
                } else {
                    // Now past whitespace (e.g. moved to non-whitespace char or end of attribute value). Either:
                    // - ignore contiguous whitespace (i.e. do nothing) if we are currently at beginning or end of value; or
                    // - collapse contiguous whitespace (i.e. count as one whitespace char) otherwise.
                    if currently_in_whitespace && !currently_first_char && char_type != CharType::End {
                        // Collect current collapsed contiguous whitespace that was ignored previously.
                        $out_char_type = CharType::Whitespace(b' ');
                        $on_char;
                    };
                    currently_in_whitespace = false;
                };
            };

            match char_type {
                CharType::End => break,
                char_type => {
                    $out_char_type = char_type;
                    $on_char;
                    currently_first_char = false;
                }
            };
        };
    };
}

pub struct ProcessedAttrValue {
    pub delimiter: DelimiterType,
    pub value: Option<ProcessorRange>,
}

pub fn process_attr_value(proc: &mut Processor, should_collapse_and_trim_ws: bool) -> ProcessingResult<ProcessedAttrValue> {
    let src_delimiter = chain!(proc.match_pred(is_attr_quote).discard().maybe_char());
    let src_delimiter_pred = match src_delimiter {
        Some(b'"') => is_double_quote,
        Some(b'\'') => is_single_quote,
        None => is_unquoted_delimiter,
        _ => unreachable!(),
    };

    // Stage 1: read and collect metrics on attribute value characters.
    let src_value_checkpoint = proc.checkpoint();
    let mut metrics = Metrics {
        count_double_quotation: 0,
        count_single_quotation: 0,
        count_whitespace: 0,
        total_whitespace_encoded_length: 0,
        first_char_type: None,
        last_char_type: None,
        collected_count: 0,
    };
    let mut char_type;
    consume_attr_value_chars!(proc, should_collapse_and_trim_ws, src_delimiter_pred, char_type, {
        metrics.collect_char_type(char_type);
    });

    // Stage 2: optimally minify attribute value using metrics.
    proc.restore(src_value_checkpoint);
    let optimal_delimiter = metrics.get_optimal_delimiter_type();
    let optimal_delimiter_char = match optimal_delimiter {
        DelimiterType::Double => Some(b'"'),
        DelimiterType::Single => Some(b'\''),
        _ => None,
    };
    // Write opening delimiter, if any.
    if let Some(c) = optimal_delimiter_char {
        proc.write(c);
    }
    let mut char_type;
    // Used to determine first and last characters.
    let mut char_no = 0usize;
    let processed_value_checkpoint = proc.checkpoint();
    consume_attr_value_chars!(proc, should_collapse_and_trim_ws, src_delimiter_pred, char_type, {
        match char_type {
            // This should never happen.
            CharType::End => unreachable!(),

            CharType::NonAsciiEntity(e) => e.keep(proc),

            CharType::Normal(c) => proc.write(c),
            // If unquoted, encode any whitespace anywhere.
            CharType::Whitespace(c) => match optimal_delimiter {
                DelimiterType::Unquoted => proc.write_slice(ENCODED[&c]),
                _ => proc.write(c),
            },
            // If single quoted, encode any single quote anywhere.
            // If unquoted, encode single quote if first character.
            CharType::SingleQuote => match (optimal_delimiter, char_no) {
                (DelimiterType::Single, _) | (DelimiterType::Unquoted, 0) => proc.write_slice(ENCODED[&b'\'']),
                _ => proc.write(b'\''),
            },
            // If double quoted, encode any double quote anywhere.
            // If unquoted, encode double quote if first character.
            CharType::DoubleQuote => match (optimal_delimiter, char_no) {
                (DelimiterType::Double, _) | (DelimiterType::Unquoted, 0) => proc.write_slice(ENCODED[&b'"']),
                _ => proc.write(b'"'),
            },
            // If unquoted, encode right chevron if last character.
            CharType::RightChevron => if optimal_delimiter == DelimiterType::Unquoted && char_no == metrics.collected_count - 1 {
                proc.write_slice(ENCODED[&b'>']);
            } else {
                proc.write(b'>');
            },
        };
        char_no += 1;
    });
    let processed_value_range = proc.written_range(processed_value_checkpoint);
    // Ensure closing delimiter in src has been matched and discarded, if any.
    if let Some(c) = src_delimiter {
        chain!(proc.match_char(c).expect().discard());
    }
    // Write closing delimiter, if any.
    if let Some(c) = optimal_delimiter_char {
        proc.write(c);
    }

    Ok(ProcessedAttrValue {
        delimiter: optimal_delimiter,
        value: Some(processed_value_range).filter(|r| !r.empty()),
    })
}
