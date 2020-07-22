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

#[inline(always)]
fn maybe_mark_indicator(line: &mut Vec<u8>, marker: u8, maybe_pos: isize, lower: usize, upper: usize) -> () {
    let pos = maybe_pos as usize;
    if maybe_pos > -1 && pos < upper {
        let pos_in_line = pos - lower;
        while line.len() <= pos_in_line {
            line.push(b' ');
        };
        line.insert(pos_in_line, if line[pos_in_line] != b' ' { b'B' } else { marker });
    };
}

// Pass -1 for read_pos or write_pos to prevent them from being represented.
pub fn debug_repr(code: &[u8], read_pos: isize, write_pos: isize) -> String {
    let mut lines = Vec::<(isize, String)>::new();
    let mut cur_pos = 0;
    for (line_no, line) in code.split(|c| *c == b'\n').enumerate() {
        // Include '\n'. Note that the last line might not have '\n' but that's OK for these calculations.
        let len = line.len() + 1;
        let line_as_string = unsafe { String::from_utf8_unchecked(line.to_vec()) };
        lines.push((line_no as isize, line_as_string));
        let new_pos = cur_pos + len;

        // Rust does lazy allocation by default, so this is not wasteful.
        let mut indicator_line = Vec::new();
        maybe_mark_indicator(&mut indicator_line, b'R', read_pos, cur_pos, new_pos);
        maybe_mark_indicator(&mut indicator_line, b'W', write_pos, cur_pos, new_pos);
        if !indicator_line.is_empty() {
            lines.push((-1, unsafe { String::from_utf8_unchecked(indicator_line) }));
        };
        cur_pos = new_pos;
    };

    let line_no_col_width = lines.len().to_string().len();
    let mut res = String::new();
    for (line_no, line) in lines {
        res.push_str(&format!(
            "{:>indent$}|{}\n",
            if line_no == -1 { ">".repeat(line_no_col_width) } else { line_no.to_string() },
            line,
            indent = line_no_col_width,
        ));
    };
    res
}
