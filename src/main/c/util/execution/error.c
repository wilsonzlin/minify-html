#include <errno.h>
#include <unistd.h>

#define HBE_COND if (*hbe_err != 0)
#define HBE_THROW(code, format, ...) *hbe_err = hbe_err_create(code, format, ##__VA_ARGS__); return 0;
#define HBE_THROW_V(code, format, ...) *hbe_err = hbe_err_create(code, format, ##__VA_ARGS__); return;
#define HBE_THROW_F(code, format, ...) *hbe_err = hbe_err_create(code, format, ##__VA_ARGS__); goto finally;
#define HBE_CATCH(fn, ...) fn(hbe_err, ##__VA_ARGS__); HBE_COND return 0;
#define HBE_CATCH_V(fn, ...) fn(hbe_err, ##__VA_ARGS__); HBE_COND return;
#define HBE_CATCH_F(fn, ...) fn(hbe_err, ##__VA_ARGS__); HBE_COND goto finally;
#define HBE_PASS(fn, ...) fn(hbe_err, ##__VA_ARGS__); return 0;
#define HBE_PASS_V(fn, ...) fn(hbe_err, ##__VA_ARGS__); return;
#define HBE_PASS_F(fn, ...) fn(hbe_err, ##__VA_ARGS__); goto finally;
#define HB_RETURN_F(v) rv = v; goto finally;

#define MAX_ERR_MSG_LEN 1024

typedef enum hbe_errcode_e {
  HBE_INTERR_UNKNOWN_ENTITY_TYPE = 2,
  HBE_INTERR_UNKNOWN_CONTENT_NEXT_STATE,
  HBE_INTERR_NOT_A_HB_DIR,

  HBE_CLI_TOO_MANY_OPTIONS = 17,
  HBE_CLI_INVALID_TAG_SET,
  HBE_CLI_INVALID_TAG,
  HBE_CLI_INVALID_SUPPRESSABLE_ERROR,

  HBE_IO_FOPEN_FAIL = 33,
  HBE_IO_FCLOSE_FAIL,
  HBE_IO_FREAD_FAIL,
  HBE_IO_FWRITE_FAIL,

  HBE_PARSE_MALFORMED_ENTITY = 65,
  HBE_PARSE_BARE_AMPERSAND,
  HBE_PARSE_INVALID_ENTITY,
  HBE_PARSE_NONSTANDARD_TAG,
  HBE_PARSE_UCASE_TAG,
  HBE_PARSE_UCASE_ATTR,
  HBE_PARSE_UNQUOTED_ATTR,
  HBE_PARSE_ILLEGAL_CHILD,
  HBE_PARSE_UNCLOSED_TAG,
  HBE_PARSE_SELF_CLOSING_TAG,
  HBE_PARSE_NO_SPACE_BEFORE_ATTR,

  HBE_PARSE_UNEXPECTED_END,
  HBE_PARSE_EXPECTED_NOT_FOUND,
} hbe_errcode_t;

typedef struct hbe_err_s {
  hbe_errcode_t code;
  char *message;
} *hbe_err_t;

hbe_err_t hbe_err_create(hbe_errcode_t code, char *format, ...) {
  va_list args;
  va_start(args, format);

  char *message = calloc(MAX_ERR_MSG_LEN + 1, SIZEOF_CHAR);
  vsnprintf(message, MAX_ERR_MSG_LEN, format, args);

  va_end(args);

  hbe_err_t err = malloc(sizeof(struct hbe_err_s));
  err->code = code;
  err->message = message;
  return err;
}

hbe_errcode_t hbe_err_code(hbe_err_t err) {
  return err->code;
}

char *hbe_err_message(hbe_err_t err) {
  return err->message;
}

void hbe_err_destroy(hbe_err_t err) {
  free(err->message);
  free(err);
}
