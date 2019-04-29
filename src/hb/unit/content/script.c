#include <hb/proc.h>

static void _parse_comment_single(hb_proc* proc)
{
	hb_proc_require_match(proc, "//");

	// Comment can end at closing </script>.
	// WARNING: Closing tag must not contain whitespace.
	while (!hb_proc_accept_if_matches_line_terminator(proc)) {
		if (hb_proc_matches_i(proc, "</script>")) {
			break;
		}

		hb_proc_accept(proc);
	}
}

static void _parse_comment_multi(hb_proc* proc)
{
	hb_proc_require_match(proc, "/*");

	// Comment can end at closing </script>.
	// WARNING: Closing tag must not contain whitespace.
	while (!hb_proc_accept_if_matches(proc, "*/")) {
		if (hb_proc_matches_i(proc, "</script>")) {
			break;
		}

		hb_proc_accept(proc);
	}
}

static void _parse_string(hb_proc* proc)
{
	hb_rune delim = hb_proc_accept(proc);

	if (delim != '"' && delim != '\'') {
		hb_proc_error(proc, HB_ERR_PARSE_EXPECTED_NOT_FOUND,
			      "Expected JavaScript string delimiter");
	}

	bool escaping = false;

	while (true) {
		hb_rune c = hb_proc_accept(proc);

		if (c == '\\') {
			escaping = !escaping;
			continue;
		}

		if (c == delim && !escaping) {
			break;
		}

		if (hb_proc_accept_if_matches_line_terminator(proc)) {
			if (!escaping) {
				hb_proc_error(proc,
					      HB_ERR_PARSE_EXPECTED_NOT_FOUND,
					      "Unterminated JavaScript string");
			}
		}

		escaping = false;
	}
}

static void _parse_template(hb_proc* proc)
{
	hb_proc_require_match(proc, "`");

	bool escaping = false;

	while (true) {
		hb_rune c = hb_proc_accept(proc);

		if (c == '\\') {
			escaping = !escaping;
			continue;
		}

		if (c == '`' && !escaping) {
			break;
		}

		escaping = false;
	}
}

void hb_unit_content_script(hb_proc* proc)
{
	while (!hb_proc_matches(proc, "</")) {
		if (hb_proc_matches(proc, "//")) {
			_parse_comment_single(proc);
		} else if (hb_proc_matches(proc, "/*")) {
			_parse_comment_multi(proc);
		} else {
			switch (hb_proc_peek(proc)) {
			case '\'':
			case '"':
				_parse_string(proc);
				break;

			case '`':
				_parse_template(proc);
				break;

			default:
				hb_proc_accept(proc);
			}
		}
	}
}
