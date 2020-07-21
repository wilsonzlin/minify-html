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

#[derive(Debug)]
pub struct Error {
    pub error_type: ErrorType,
    pub position: usize,
}

#[derive(Debug)]
pub struct FriendlyError {
    pub position: usize,
    pub message: String,
    pub code_context: String,
}

pub type ProcessingResult<T> = Result<T, ErrorType>;
