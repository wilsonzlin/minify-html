pub enum ErrorType {
    EntityFollowingMalformedEntity,
    NoSpaceBeforeAttr,
    UnterminatedCssString,
    UnterminatedJsString,
    UnterminatedJsRegExp,
    CharNotFound { need: u8, got: u8 },
    MatchNotFound(&'static [u8]),
    NotFound(&'static str),
    ExpectedChar(u8),
    UnexpectedEnd,
}

impl ErrorType {
    pub fn message(self) -> String {
        match self {
            ErrorType::EntityFollowingMalformedEntity => {
                format!("Entity cannot follow malformed entity.")
            }
            ErrorType::NoSpaceBeforeAttr => {
                format!("Space required before attribute.")
            }
            ErrorType::UnterminatedCssString => {
                format!("Unterminated CSS string.")
            }
            ErrorType::UnterminatedJsString => {
                format!("Unterminated JavaScript string.")
            }
            ErrorType::UnterminatedJsRegExp => {
                format!("Unterminated JavaScript regular expression.")
            }
            ErrorType::CharNotFound { need, got } => {
                format!("Expected {} (U+{:X}), got {} (U+{:X}).", need as char, need, got as char, got)
            }
            ErrorType::MatchNotFound(seq) => {
                format!("Expected `{}`.", unsafe { std::str::from_utf8_unchecked(seq) })
            }
            ErrorType::NotFound(exp) => {
                format!("Expected {}.", exp)
            }
            ErrorType::ExpectedChar(unexp) => {
                format!("Expected {} (U+{:X}).", unexp as char, unexp)
            }
            ErrorType::UnexpectedEnd => {
                format!("Unexpected end of source code.")
            }
        }
    }
}

pub type ProcessingResult<T> = Result<T, ErrorType>;
