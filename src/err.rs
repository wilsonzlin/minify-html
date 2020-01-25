// Implement debug to allow .unwrap().
#[derive(Debug)]
pub enum ErrorType {
    ClosingTagMismatch,
    NotFound(&'static str),
    UnexpectedEnd,
}

impl ErrorType {
    pub fn message(self) -> String {
        match self {
            ErrorType::ClosingTagMismatch => {
                format!("Closing tag name does not match opening tag.")
            }
            ErrorType::NotFound(exp) => {
                format!("Expected {}.", exp)
            }
            ErrorType::UnexpectedEnd => {
                format!("Unexpected end of source code.")
            }
        }
    }
}

pub type ProcessingResult<T> = Result<T, ErrorType>;
