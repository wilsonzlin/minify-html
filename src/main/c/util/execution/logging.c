#define PC_RED   "\e[31m"
#define PC_GRN   "\e[32m"
#define PC_YEL   "\e[33m"
#define PC_BLU   "\e[34m"
#define PC_MAG   "\e[35m"
#define PC_CYN   "\e[36m"
#define PC_WHT   "\e[37m"
#define PC_RESET "\e[0m"
#define PC_BOLD  "\e[1m"

#define HBL_LOG_DEBUG_PREFIX "[DEBUG] "
#define HBL_LOG_INFO_PREFIX PC_MAG "[INFO] "
#define HBL_LOG_WARN_PREFIX PC_BOLD PC_YEL "[WARN] "

#define HBL_KV_LOGLINE_LEFTWIDTH "35"

typedef enum hbl_log_level_e {
  HBL_LOG_DEBUG,
  HBL_LOG_INFO,
  HBL_LOG_WARN,
} hbl_log_level_t;

void hbl_log(hbl_log_level_t level, char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  const char *prefix = level == HBL_LOG_INFO ? (HBL_LOG_INFO_PREFIX) :
                       level == HBL_LOG_WARN ? (HBL_LOG_WARN_PREFIX) :
                       HBL_LOG_DEBUG_PREFIX;

  fprintf(stderr, "%s" PC_RESET, prefix);
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");

  va_end(args);
}

void hbl_info_kv_boolean(char *name, int state) {
  const char *color = state ? (PC_BOLD PC_GRN) : PC_MAG;
  const char *label = state ? "ON" : "OFF";

  hbl_log(HBL_LOG_INFO, "%-" HBL_KV_LOGLINE_LEFTWIDTH "s%s%s" PC_RESET, name, color, label);
}

void hbl_info_kv_string(char *name, char *value) {
  hbl_log(HBL_LOG_INFO, "%-" HBL_KV_LOGLINE_LEFTWIDTH "s" PC_BOLD "%s" PC_RESET, name, value);
}

void hbl_error(hbe_err_t err) {
  fprintf(stderr, PC_BOLD PC_RED "[FATAL]" PC_RESET PC_RED " Error %d: %s\n" PC_RESET, err->code, err->message);
}
