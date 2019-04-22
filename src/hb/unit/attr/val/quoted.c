#include <hb/proc.h>
#include <hb/rule.h>
#include <hb/unit.h>

#define _ENCODED_SINGLE_QUOTE "&#39;"
#define _ENCODED_DOUBLE_QUOTE "&#34;"

#define _COLLAPSE_WHITESPACE_IF_APPLICABLE() \
            if (last_char_was_whitespace) { \
                /* This is the first non-whitespace character after one or more whitespace character(s), so collapse whitespace by writing only one space. */ \
                hb_proc_write(proc, ' '); \
                has_whitespace_after_processing = true; \
                last_char_was_whitespace = false; \
            }

hb_unit_attr_type hb_unit_attr_val_quoted(hb_proc* proc, bool should_collapse_and_trim_ws) {
    // Processing a quoted attribute value is tricky, due to the fact that it's not possible to know whether or not to unquote the value until the value has been processed.
    // For example, decoding an entity could create whitespace in a value which might otherwise be unquotable.
    // How this function works is:
    //
    // 1. Assume that the value is unquotable, and don't output any quotes. Decode any entities as necessary. Collect metrics on the types of characters in the value while processing.
    // 2. Based on the metrics, if it's possible to not use quotes, nothing needs to be done and the function ends.
    // 3. Choose a quote based on the amount of occurrences, to minimise the amount of encoded values.
    // 4. Post-process the output by adding delimiter quotes and encoding quotes in values. This does mean that the output is written to twice.

    bool should_decode_entities = proc->cfg->decode_entities;
    bool should_remove_quotes = proc->cfg->remove_attr_quotes;

    // Metrics for characters in the value.
    // Used to decide what quotes to use, if any.
    size_t count_double_quotation = 0;
    size_t count_single_quotation = 0;
    bool starts_with_quote = false;
    bool has_whitespace_after_processing = false;

    hb_rune quote = hb_proc_require_skip_predicate(proc, &hb_rule_attr_quote_check, "attribute value quote");

    if (should_collapse_and_trim_ws) {
        hb_proc_skip_while_predicate(proc, &hb_rule_ascii_whitespace_check);
    }

    // Since it's not possible to optimise the delimiter quotes without knowing the complete value,
    // mark the processed value in the output for post-processing later.
    hb_proc_view_init_out(proc_value, proc);

    hb_proc_view_start_with_out_next(&proc_value, proc);
    bool last_char_was_whitespace = false;
    bool is_first_char = true;
    while (true) {
        int32_t c = hb_proc_peek(proc);

        if (c == quote) {
            break;
        }

        bool processed_entity = c == '&';
        if (processed_entity) {
            // If not decoding entities, then this is first non-whitespace if last_char_was_whitespace, so space needs to be written before hb_unit_entity writes entity.
            if (!should_decode_entities) {
                _COLLAPSE_WHITESPACE_IF_APPLICABLE()
            }

            // Characters will be consumed by hb_unit_entity, but they will never be '\'', '"', or whitespace,
            // as the function only consumes characters that could form a well formed entity.
            // See the function for more details.
            int32_t decoded = hb_unit_entity(proc);
            // If not decoding entities, don't interpret using decoded character.
            if (should_decode_entities) c = decoded;
        }
        bool is_whitespace = hb_rule_ascii_whitespace_check(c);

        if (should_collapse_and_trim_ws && is_whitespace) {
            // Character, after any entity decoding, is whitespace.
            // Don't write whitespace.
            // In order to collapse whitespace, only write one space character once the first non-whitespace character after a sequence of whitespace characters is reached.
            last_char_was_whitespace = true;
            hb_proc_skip(proc);

        } else {
            // Character, after any entity decoding, is not whitespace.
            _COLLAPSE_WHITESPACE_IF_APPLICABLE()

            if (c == '"') {
                if (is_first_char) starts_with_quote = true;
                count_double_quotation++;

            } else if (c == '\'') {
                if (is_first_char) starts_with_quote = true;
                count_single_quotation++;

            } else if (is_whitespace) {
                // `should_collapse_and_trim_ws` is false, so whitespace is written.
                has_whitespace_after_processing = true;
            }

            if (!processed_entity) {
                // Don't need to accept if hb_unit_entity has already been called.
                hb_proc_accept(proc);
            }
        }

        is_first_char = false;
    }
    hb_proc_view_end_with_out_prev(&proc_value, proc);
    hb_proc_require_skip(proc, quote);

    size_t proc_length = nh_view_str_length(&proc_value);

    // Technically, the specification states that values may only be unquoted if they don't contain ["'`=<>].
    // However, browsers seem to interpret characters after `=` and before the nearest whitespace as an unquoted value, so long as no quote immediately follows `=`.
    // If a value cannot be unquoted, use the one that appears the least and therefore requires the least amount of encoding.
    // Prefer double quotes to single quotes if it's a tie.
    hb_rune quote_to_encode;
    char const* quote_encoded;
    size_t quote_encoded_length;
    size_t amount_of_quotes_to_encode;

    if (should_remove_quotes && proc_length > 0 && !has_whitespace_after_processing && !starts_with_quote) {
        // No need to do any further processing; processed value is already in unquoted form.
        return HB_UNIT_ATTR_UNQUOTED;

    } else if (!should_decode_entities) {
        // If entities are not being decoded, we are not allowed to encode and decode quotes to minimise the total count of encoded quotes.
        // Therefore, there is no use to swapping delimiter quotes as at best it's not an improvement and at worst it could break the value.
        quote_to_encode = quote;
        quote_encoded = NULL;
        quote_encoded_length = 0;
        amount_of_quotes_to_encode = 0;

    } else if (count_single_quotation < count_double_quotation) {
        quote_to_encode = '\'';
        quote_encoded = _ENCODED_SINGLE_QUOTE;
        quote_encoded_length = hb_string_literal_length(_ENCODED_SINGLE_QUOTE);
        amount_of_quotes_to_encode = count_single_quotation;

    } else {
        quote_to_encode = '"';
        quote_encoded = _ENCODED_DOUBLE_QUOTE;
        quote_encoded_length = hb_string_literal_length(_ENCODED_DOUBLE_QUOTE);
        amount_of_quotes_to_encode = count_double_quotation;
    }

    size_t post_length = 2 + proc_length - amount_of_quotes_to_encode + (amount_of_quotes_to_encode * quote_encoded_length);
    // Where the post-processed output should start in the output array.
    size_t out_start = nh_view_str_start(&proc_value);
    size_t proc_end = out_start + proc_length - 1;
    size_t post_end = out_start + post_length - 1;

    size_t reader = proc_end;
    size_t writer = post_end;
    proc->out[writer--] = quote_to_encode;
    // To prevent overwriting data when encoding quotes, post-process output in reverse.
    // Loop condition is checked at end of loop instead of before to prevent underflow.
    // WARNING: This code directly uses and manipulates struct members of `proc`, which in general should be avoided.
    while (true) {
        hb_rune c = proc->out[reader];
        if (should_decode_entities && c == quote_to_encode) {
            writer -= quote_encoded_length;
            // WARNING: This only works because hb_rune == char.
            memcpy(&proc->out[writer + 1], quote_encoded, quote_encoded_length * sizeof(hb_rune));
        } else {
            proc->out[writer--] = c;
        }

        // Break before decrementing to prevent underflow.
        if (reader == out_start) {
            break;
        }
        reader--;
    }
    // This must be done after previous loop to prevent overwriting data.
    proc->out[writer] = quote_to_encode;
    proc->out_next = post_end + 1;

    return HB_UNIT_ATTR_QUOTED;
}
