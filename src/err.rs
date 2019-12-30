#[derive(Debug)]
pub enum ErrorType {
    EntityFollowingMalformedEntity,
    NoSpaceBeforeAttr,
    UnterminatedCssString,
    UnterminatedJsString,
    CharNotFound { need: u8, got: u8 },
    MatchNotFound(&'static [u8]),
    NotFound(&'static str),
    UnexpectedChar(u8),
    UnexpectedEnd,
}

pub type ProcessingResult<T> = Result<T, ErrorType>;
