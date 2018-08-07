#ifndef _HDR_HYPERBUILD_STREAM_CONTENT
#define _HDR_HYPERBUILD_STREAM_CONTENT

#include "../rule/char/whitespace.c"

#include "../util/hbchar.h"
#include "../util/pipe.c"

// Declare first before tag.c, as tag.c depends on it
void hbs_content(hbs_options_t so, hbu_pipe_t pipe, hb_char_t *parent);

#include "./streamoptions.c"
#include "./tag.c"
#include "./bang.c"
#include "./comment.c"
#include "./entity.c"

#define HBS_CONTENT_NEXT_STATE_END 1
#define HBS_CONTENT_NEXT_STATE_COMMENT 2
#define HBS_CONTENT_NEXT_STATE_BANG 3
#define HBS_CONTENT_NEXT_STATE_OPENING_TAG 4
#define HBS_CONTENT_NEXT_STATE_ENTITY 5
#define HBS_CONTENT_NEXT_STATE_TEXT 6

static int _hbs_content_state_is_comment_bang_or_opening_tag(int state) {
  return state == HBS_CONTENT_NEXT_STATE_COMMENT ||
         state == HBS_CONTENT_NEXT_STATE_BANG ||
         state == HBS_CONTENT_NEXT_STATE_OPENING_TAG;
}

// $parent can be NULL for top-level content
void hbs_content(hbs_options_t so, hbu_pipe_t pipe, hb_char_t *parent) {
  int is_first_char = 1;
  // Set to 1 when $whitespace is instantiated when $is_first_char is 1
  int whitespace_buffer_started_at_beginning = 0;

  // Set to one after calling hbs_comment, hbs_bang, or hbs_tag
  int returned_from_comment_bang_or_tag = 0;
  int whitespace_buffer_started_after_right_chevron = 0;

  int should_collapse_whitespace = !hbs_options_in_tags_list(so->ex_collapse_whitespace, parent);
  int should_destroy_whole_whitespace = !hbs_options_in_tags_list(so->ex_destroy_whole_whitespace, parent);
  int should_trim_whitespace = !hbs_options_in_tags_list(so->ex_trim_whitespace, parent);

  int should_buffer_whitespace = should_collapse_whitespace || should_destroy_whole_whitespace || should_trim_whitespace;
  hbu_buffer_t whitespace = NULL;

  while (1) {
    hb_eod_char_t c = hbu_pipe_peek_eoi(pipe);

    int next_state;

    if (c == HB_EOD || hbu_pipe_matches(pipe, "</")) {
      // End of content
      next_state = HBS_CONTENT_NEXT_STATE_END;

    } else if (hbu_pipe_matches(pipe, "<!--")) {
      // Comment
      next_state = HBS_CONTENT_NEXT_STATE_COMMENT;

    } else if (hbu_pipe_matches(pipe, "<!")) {
      // Bang
      // NOTE: Check after comment
      next_state = HBS_CONTENT_NEXT_STATE_BANG;

    } else if (c == '<') {
      // Opening tag
      // NOTE: Check after comment and bang
      next_state = HBS_CONTENT_NEXT_STATE_OPENING_TAG;

    } else if (c == '&') {
      // Entity
      next_state = HBS_CONTENT_NEXT_STATE_ENTITY;

    } else {
      // Text
      next_state = HBS_CONTENT_NEXT_STATE_TEXT;
    }

    if (next_state == HBS_CONTENT_NEXT_STATE_TEXT && hbr_whitespace_check(c) && should_buffer_whitespace) {
      // Next character is whitespace and whitespace should be buffered
      if (whitespace == NULL) {
        whitespace = hbu_buffer_create();
        whitespace_buffer_started_at_beginning = is_first_char;
        whitespace_buffer_started_after_right_chevron = returned_from_comment_bang_or_tag;
      }
      hbu_buffer_append(whitespace, c);
      hbu_pipe_skip(pipe);

    } else {
      if (whitespace != NULL) {
        // Next character is not whitespace, deal with existing buffered whitespace
        if (should_destroy_whole_whitespace &&
            whitespace_buffer_started_after_right_chevron &&
            _hbs_content_state_is_comment_bang_or_opening_tag(next_state)
        ) {
          // Do nothing

        } else if (should_trim_whitespace &&
                   (whitespace_buffer_started_at_beginning || next_state == HBS_CONTENT_NEXT_STATE_END)) {
          // Do nothing

        } else if (should_collapse_whitespace) {
          hbu_pipe_write(pipe, ' ');
        }

        hbu_buffer_destroy(whitespace);
        whitespace = NULL;
        whitespace_buffer_started_at_beginning = 0;
      }

      switch (next_state) {
      case HBS_CONTENT_NEXT_STATE_TEXT:
        hbu_pipe_accept(pipe);
        break;

      case HBS_CONTENT_NEXT_STATE_COMMENT:
        hbs_comment(so, pipe);
        break;

      case HBS_CONTENT_NEXT_STATE_BANG:
        hbs_bang(pipe);
        break;

      case HBS_CONTENT_NEXT_STATE_OPENING_TAG:
        hbs_tag(so, pipe, parent);
        break;

      case HBS_CONTENT_NEXT_STATE_ENTITY:
        hbs_entity(so, pipe);
        break;

      case HBS_CONTENT_NEXT_STATE_END:
        return;

      default:
        hbe_fatal(HBE_INTERR_UNKNOWN_CONTENT_NEXT_STATE, "INTERR $next_state is not a known upcoming content stream state");
      }

      returned_from_comment_bang_or_tag = _hbs_content_state_is_comment_bang_or_opening_tag(next_state);
    }

    is_first_char = 0;
  }
}

#endif // _HDR_HYPERBUILD_STREAM_CONTENT
