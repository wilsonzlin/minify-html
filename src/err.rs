// Implement debug to allow .unwrap().
#[derive(Debug)]
pub enum ErrorType {
    ClosingTagMismatch,
    MatchNotFound(&'static [u8]),
    NotFound(&'static str),
    ExpectedChar(u8),
    UnexpectedEnd,
}

impl ErrorType {
    pub fn message(self) -> String {
        match self {
            ErrorType::ClosingTagMismatch => {
                format!("Closing tag name does not match opening tag.")
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
