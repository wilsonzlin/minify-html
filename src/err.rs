#[derive(Debug)]
pub enum ErrorType {
    CharNotFound { need: u8, got: u8 },
    MatchNotFound(&'static [u8]),
    NotFound(&'static str),
    NoSpaceBeforeAttr,
    UnexpectedChar(u8),
    UnexpectedEnd,
}

pub type InternalResult<T> = Result<T, ErrorType>;
