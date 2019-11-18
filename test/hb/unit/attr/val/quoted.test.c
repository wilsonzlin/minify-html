#include <hb/hyperbuild.h>
#include <hb/unit.h>
#include <hbtest.h>
#include <stdio.h>
#include <string.h>

// An attribute value:
//  - delimited by double quotes
//  - containing one single quote literal
//  - containing one single quote encoded
//  - containing three double quotes encoded
//  - with multiple whitespace sequences of length 2 and higher, including at
//  the start and end
#define INPUT "\"  abc&apos;'&quot; &quot;&quot;    a  \" 1"

int main(void)
{
	hyperbuild_init();

	hb_err_set* suppressed = hb_err_set_create();

	hb_rune* src = malloc(sizeof(INPUT) + 1);
	memcpy(src, INPUT "\xFF", sizeof(INPUT) + 1);

	hb_cfg cfg = {
		.collapse_whitespace =
			{
				.mode = HB_CFG_TAGS_SET_MODE_ALL,
				.set = NULL,
			},
		.destroy_whole_whitespace =
			{
				.mode = HB_CFG_TAGS_SET_MODE_ALL,
				.set = NULL,
			},
		.trim_whitespace =
			{
				.mode = HB_CFG_TAGS_SET_MODE_ALL,
				.set = NULL,
			},
		.suppressed_errors = *suppressed,
		.trim_class_attributes = true,
		.decode_entities = true,
		.remove_attr_quotes = true,
		.remove_comments = true,
		.remove_tag_whitespace = true,
	};

	hb_proc proc = {
		.cfg = &cfg,
		.src = src,
		.src_len = sizeof(INPUT) - 1,
		.src_next = 0,
		.out = src,
		.out_next = 0,
	};

	hb_unit_attr_val_quoted(&proc, true);

	src[proc.out_next] = 0;
	printf("%s\n", src);

	hb_err_set_destroy(suppressed);
}
