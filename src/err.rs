pub enum HbErr {
    ExpectedCharNotFound { expected: u8, got: u8 },
    ExpectedMatchNotFound(&'static [u8]),
    ExpectedNotFound(&'static str),
    NoSpaceBeforeAttr,
    UnclosedTag,
    UnexpectedCharFound(u8),
    UnexpectedEnd,
}

pub type HbRes<T> = Result<T, HbErr>;
