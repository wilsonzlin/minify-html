// Implement debug to allow .unwrap().
#[derive(Debug)]
pub enum HbErr {
    ClosingTagMismatch,
    Unexpected(usize),
    UnexpectedEnd,
}

impl HbErr {
    pub fn message(self) -> String {
        match self {
            HbErr::ClosingTagMismatch => {
                format!("Closing tag name does not match opening tag.")
            }
            HbErr::Unexpected(pos) => {
                format!("Unexpected character at position {}.", pos)
            }
            HbErr::UnexpectedEnd => {
                format!("Unexpected end of source code.")
            }
        }
    }
}

pub type HbRes<T> = Result<T, HbErr>;
