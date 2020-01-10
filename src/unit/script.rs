use crate::err::ProcessingResult;
use crate::proc::Processor;

// See https://www.w3.org/TR/html52/syntax.html#script-data-state and "notes/Script data.md".

enum State {
    End,
    Normal,
    Escaped,
    DoubleEscaped,
}

// TODO Optimise all functions in this file.

fn process_script_data_double_escaped(proc: &mut Processor) -> ProcessingResult<State> {
    loop {
        if chain!(proc.match_seq(b"</script").keep().matched()) {
            return Ok(State::Escaped);
        };
        if chain!(proc.match_seq(b"-->").keep().matched()) {
            return Ok(State::Normal);
        };
        proc.accept()?;
    };
}

fn process_script_data_escaped(proc: &mut Processor) -> ProcessingResult<State> {
    loop {
        if chain!(proc.match_seq(b"<script").keep().matched()) {
            return Ok(State::DoubleEscaped);
        };
        if chain!(proc.match_seq(b"</script").matched()) {
            return Ok(State::End);
        };
        if chain!(proc.match_seq(b"-->").keep().matched()) {
            return Ok(State::Normal);
        };
        proc.accept()?;
    };
}

fn process_script_data(proc: &mut Processor) -> ProcessingResult<State> {
    loop {
        if chain!(proc.match_seq(b"</script").matched()) {
            return Ok(State::End);
        };
        if chain!(proc.match_seq(b"<!--").keep().matched()) {
            return Ok(State::Escaped);
        };
        proc.accept()?;
    };
}

pub fn process_script(proc: &mut Processor) -> ProcessingResult<()> {
    // NOTE: See "notes/Script data.md".
    let mut state = State::Normal;
    loop {
        state = match state {
            State::End => break,
            State::Normal => process_script_data(proc)?,
            State::Escaped => process_script_data_escaped(proc)?,
            State::DoubleEscaped => process_script_data_double_escaped(proc)?,
        };
    };
    Ok(())
}
