// Implement debug to allow .unwrap().
#[derive(Debug)]
pub enum ErrorType {
    EntityFollowingMalformedEntity,
    ClosingTagMismatch,
    NoSpaceBeforeAttr,
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
            ErrorType::ClosingTagMismatch => {
                format!("Closing tag name does not match opening tag.")
            }
            ErrorType::NoSpaceBeforeAttr => {
                format!("Space required before attribute.")
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
