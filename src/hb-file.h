#pragma once

#include <errno.h>

typedef enum {
	HB_FILE_ENC_UTF_8,
	HB_FILE_ENC_UTF_16,
} hb_file_enc_t;

#define HB_FILE(type, mode, noun, verb, std)                   \
	typedef struct {                                 \
		char const* name;                                              \
		hb_file_enc_t encoding; \
		FILE* fd;                                                      \
	} hb_file_##type##_t;                                             \
                                                                               \
	hb_file_##type##_t* hb_file_##type##_create(char* path)           \
	{                                                                      \
		hb_file_##type##_t* fstream =                                \
			malloc(sizeof(hb_file_##type##_t));          \
                                                                               \
		if (path == NULL) {                                            \
			fstream->name = #std;                                  \
			fstream->fd = std;                                     \
		} else {                                                       \
			fstream->name = path;                                  \
                                                                               \
			FILE* fd = fopen(path, mode);                          \
                                                                               \
			if (fd == NULL) {                                      \
				return NULL;               \
			}                                                      \
                                                                               \
			fstream->fd = fd;                                      \
		}                                                              \
                                                                               \
		return fstream;                                                \
	}                                                                      \
                                                                               \
	void hb_file_##type##_destroy(hbe_err_t* hbe_err,                   \
					 hb_file_##type##_t fstream)        \
	{                                                                      \
		if (fclose(fstream->fd) == EOF) {                              \
			HBE_THROW_V(HBE_IO_FCLOSE_FAIL,                        \
				    "Failed to close " noun                    \
				    " stream for file %s with error %d",       \
				    fstream->name, errno);                     \
		}                                                              \
                                                                               \
		free(fstream);                                                 \
	}
