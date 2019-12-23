fn tmp() -> () {
    // TODO
    loop {
        let is_whitespace = is_whitespace(c);
        if should_collapse_and_trim_ws && is_whitespace {
            // Character, after any entity decoding, is whitespace.
            // Don't write whitespace.
            // In order to collapse whitespace, only write one space
            // character once the first non-whitespace character
            // after a sequence of whitespace characters is reached.
            last_char_was_whitespace = true;
            proc.skip();
        } else {
            // Character, after any entity decoding, is not whitespace.
            if last_char_was_whitespace {
                // This is the first non-whitespace character after one or more whitespace
                // character(s), so collapse whitespace by writing only one space.
                proc.write(b' ');
                has_whitespace_after_processing = true;
                last_char_was_whitespace = false;
            };

            if c == b'"' {
                count_double_quotation += 1;
            } else if c == b'\'' {
                count_single_quotation += 1;
            } else if is_whitespace {
                // `should_collapse_and_trim_ws` is false, so
                // whitespace is written.
                has_whitespace_after_processing = true;
            };

            increment_count(c);
            if !processed_entity {
                // Don't need to accept if hb_unit_entity has
                // already been called.
                proc.accept();
            };
        };
    }

    // Since it's not possible to optimise the delimiter quotes without
    // knowing the complete value, mark the processed value in the output
    // for post-processing later.
    let proc_value_start = proc.data.get_out_pos();
    let mut is_first_char = true;

    loop {
        let processed_entity = c == b'&';
        if processed_entity {
            // Characters will be consumed by hb_unit_entity, but they will never be '\'', '"', or
            // whitespace, as the function only consumes characters that could form a  well formed
            // entity. See the function for more details.
            // TODO Handle bad char
            let decoded = process_entity(proc)?;
            match decoded {
                Some(e) => if e <= 0x7f { c = e as u8; } else { c = 0xff; },
                None => c = 0xff,
            };
        }


        is_first_char = false;
    };
    let proc_length = proc.data.get_out_pos() + 1 - proc_value_start;
    proc.match_char(delimiter).require()?.discard();

    // Technically, the specification states that values may only be
    // unquoted if they don't contain ["'`=<>]. However, browsers seem to
    // interpret characters after `=` and before the nearest whitespace as
    // an unquoted value, so long as no quote immediately follows `=`. If a
    // value cannot be unquoted, use the one that appears the least and
    // therefore requires the least amount of encoding. Prefer double quotes
    // to single quotes if it's a tie.
    let quote_to_encode;
    let quote_encoded;
    let amount_of_quotes_to_encode;

    if proc_length > 0 && !has_whitespace_after_processing && !starts_with_quote {
        // No need to do any further processing; processed value is
        // already in unquoted form.
        return Ok(AttrType::Unquoted);
    } else if count_single_quotation < count_double_quotation {
        quote_to_encode = b'\'';
        quote_encoded = ENCODED_SINGLE_QUOTE;
        amount_of_quotes_to_encode = count_single_quotation;
    } else {
        quote_to_encode = b'"';
        quote_encoded = ENCODED_DOUBLE_QUOTE;
        amount_of_quotes_to_encode = count_double_quotation;
    }

    // TODO Improve; avoid direct memory access; clean API.
    let post_length = 2 + proc_length - amount_of_quotes_to_encode + (amount_of_quotes_to_encode * quote_encoded.len());
    // Where the post-processed output should start in the output array.
    let out_start = proc_value_start;
    let proc_end = out_start + proc_length - 1;
    let post_end = out_start + post_length - 1;

    let mut reader = proc_end;
    let mut writer = post_end;
    proc.data.set_out_char_at(writer, quote_to_encode);
    writer -= 1;
    // To prevent overwriting data when encoding quotes, post-process output
    // in reverse. Loop condition is checked at end of loop instead of
    // before to prevent underflow. WARNING: This code directly uses and
    // manipulates struct members of `proc`, which in general should be
    // avoided.
    loop {
        let c = proc.data.get_src_char_at(reader);
        if c == quote_to_encode {
            writer -= quote_encoded.len();
            proc.data.replace_out_slice(writer + 1, quote_encoded);
        } else {
            proc.data.set_out_char_at(writer, c);
            writer -= 1;
        }

        // Break before decrementing to prevent underflow.
        if reader == out_start {
            break;
        }
        reader -= 1;
    }
    // This must be done after previous loop to prevent overwriting data.
    proc.data.set_out_char_at(writer, quote_to_encode);
    proc.data.set_out_pos(post_end + 1);

    Ok(AttrType::Quoted)
}
