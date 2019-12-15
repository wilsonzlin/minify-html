#include <hb/proc.h>

static void _parse_comment(hb_proc* proc)
{
	hb_proc_require_match(proc, "/*");

	// Unlike script tags, style comments do NOT end at closing tag.
	while (!hb_proc_accept_if_matches(proc, "*/")) {
		hb_proc_accept(proc);
	}
}

static void _parse_string(hb_proc* proc)
{
	hb_rune delim = hb_proc_accept(proc);

	if (delim != '"' && delim != '\'') {
		hb_proc_error(proc, HB_ERR_PARSE_EXPECTED_NOT_FOUND,
			      "Expected CSS string delimiter");
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
					      "Unterminated CSS string");
			}
		}

		escaping = false;
	}
}

void hb_unit_content_style(hb_proc* proc)
{
	while (!hb_proc_matches(proc, "</")) {
		if (hb_proc_matches(proc, "/*")) {
			_parse_comment(proc);
		} else {
			switch (hb_proc_peek(proc)) {
			case '\'':
			case '"':
				_parse_string(proc);
				break;
			default:
				hb_proc_accept(proc);
			}
		}
	}
}
