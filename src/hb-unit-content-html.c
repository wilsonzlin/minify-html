// Declare first before tag.c, as tag.c depends on it
void hbs_content(hbe_err_t *hbe_err, hbu_streamoptions_t so, hb_proc_t pipe, hb_proc_char_t *parent);

#include "../tag/tag.c"
#include "../bang/bang.c"
#include "../comment/comment.c"
#include "../entity/entity.c"

// $parent can be NULL for top-level content
void hbs_content(hbe_err_t *hbe_err, hbu_streamoptions_t so, hb_proc_t pipe, hb_proc_char_t *parent) {
  int is_first_char = 1;
  // Set to 1 when $whitespace is instantiated when $is_first_char is 1
  int whitespace_buffer_started_at_beginning = 0;

  // Set to one after calling hbs_comment, hbs_bang, or hbs_tag
  int returned_from_comment_bang_or_tag = 0;
  int whitespace_buffer_started_after_right_chevron = 0;

  int should_collapse_whitespace = !hbu_streamoptions_in_tags_list(so->ex_collapse_whitespace, parent);
  int should_destroy_whole_whitespace = !hbu_streamoptions_in_tags_list(so->ex_destroy_whole_whitespace, parent);
  int should_trim_whitespace = !hbu_streamoptions_in_tags_list(so->ex_trim_whitespace, parent);

  int should_buffer_whitespace = should_collapse_whitespace || should_destroy_whole_whitespace || should_trim_whitespace;
  hb_list_char_t whitespace = NULL;

  while (1) {
    hb_eod_char_t c = HBE_CATCH_F(hb_proc_peek_eoi, pipe);
    int next_state = HBE_CATCH_F(_hbs_content_get_next_state, pipe, c);

    if (next_state == HBS_CONTENT_STATE_TEXT && hb_rule_whitespace_check(c) && should_buffer_whitespace) {
      // Next character is whitespace and whitespace should be buffered
      if (whitespace == NULL) {
        whitespace = hb_list_char_create();
        whitespace_buffer_started_at_beginning = is_first_char;
        whitespace_buffer_started_after_right_chevron = returned_from_comment_bang_or_tag;
      }
      hb_list_char_append(whitespace, c);
      HBE_CATCH_F(hb_proc_skip, pipe);

    } else {
      if (whitespace != NULL) {
        // Next character is not whitespace, deal with existing buffered whitespace
        if (should_destroy_whole_whitespace &&
            whitespace_buffer_started_after_right_chevron &&
            _hbs_content_state_is_comment_bang_or_opening_tag(next_state)
        ) {
          // Do nothing

        } else if (should_trim_whitespace &&
                   (whitespace_buffer_started_at_beginning || next_state == HBS_CONTENT_STATE_END)) {
          // Do nothing

        } else if (should_collapse_whitespace) {
          HBE_CATCH_F(hb_proc_write, pipe, ' ');
        }

        hb_list_char_destroy(whitespace);
        whitespace = NULL;
        whitespace_buffer_started_at_beginning = 0;
      }

      switch (next_state) {
      case HBS_CONTENT_STATE_TEXT:
        HBE_CATCH_F(hb_proc_accept, pipe);
        break;

      case HBS_CONTENT_STATE_COMMENT:
        HBE_CATCH_F(hbs_comment, so, pipe);
        break;

      case HBS_CONTENT_STATE_BANG:
        HBE_CATCH_F(hbs_bang, pipe);
        break;

      case HBS_CONTENT_STATE_OPENING_TAG:
        HBE_CATCH_F(hbs_tag, so, pipe, parent);
        break;

      case HBS_CONTENT_STATE_ENTITY:
        HBE_CATCH_F(hbs_entity, so, pipe);
        break;

      case HBS_CONTENT_STATE_END:
        return;

      default:
        HBE_THROW_F(HBE_INTERR_UNKNOWN_CONTENT_NEXT_STATE, "INTERR $next_state is not a known upcoming content stream state");
      }

      returned_from_comment_bang_or_tag = _hbs_content_state_is_comment_bang_or_opening_tag(next_state);
    }

    is_first_char = 0;
  }

  finally:
    if (whitespace != NULL) {
      hb_list_char_destroy(whitespace);
      whitespace = NULL;
    }
}
