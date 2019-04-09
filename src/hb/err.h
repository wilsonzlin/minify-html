#pragma once

typedef enum {
	// WARNING: The __HB_NUMBER_OF_ERROR_CODES value only works if the first value of this enum is set to zero.
	HB_ERR_OK = 0,

	HB_ERR_INTERR_UNKNOWN_ENTITY_TYPE,
	HB_ERR_INTERR_UNKNOWN_CONTENT_NEXT_STATE,

	HB_ERR_CLI_TOO_MANY_OPTIONS,
	HB_ERR_CLI_INVALID_TAG_SET,
	HB_ERR_CLI_INVALID_TAG,
	HB_ERR_CLI_INVALID_SUPPRESSABLE_ERROR,

	HB_ERR_IO_FOPEN_FAIL,
	HB_ERR_IO_FCLOSE_FAIL,
	HB_ERR_IO_FREAD_FAIL,
	HB_ERR_IO_FWRITE_FAIL,

	HB_ERR_PARSE_MALFORMED_ENTITY,
	HB_ERR_PARSE_BARE_AMPERSAND,
	HB_ERR_PARSE_INVALID_ENTITY,
	HB_ERR_PARSE_NONSTANDARD_TAG,
	HB_ERR_PARSE_UCASE_TAG,
	HB_ERR_PARSE_UCASE_ATTR,
	HB_ERR_PARSE_UNQUOTED_ATTR,
	HB_ERR_PARSE_ILLEGAL_CHILD,
	HB_ERR_PARSE_UNCLOSED_TAG,
	HB_ERR_PARSE_SELF_CLOSING_TAG,
	HB_ERR_PARSE_NO_SPACE_BEFORE_ATTR,

	HB_ERR_PARSE_UNEXPECTED_END,
	HB_ERR_PARSE_EXPECTED_NOT_FOUND,

	// Special value to represent the amount of values above in this enum.
	// WARNING: This only works if the first value is set to zero.
	__HB_NUMBER_OF_ERROR_CODES,
} hb_err;

// Set of error codes. Used for suppressing errors.
typedef struct {
	bool set[__HB_NUMBER_OF_ERROR_CODES];
} hb_err_set;

hb_err_set* hb_err_set_create(void);
void hb_err_set_add(hb_err_set* set, hb_err err);
void hb_err_set_remove(hb_err_set* set, hb_err err);
bool hb_err_set_has(hb_err_set* set, hb_err err);
