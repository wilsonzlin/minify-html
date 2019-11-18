#pragma once

#include <setjmp.h>

#include "hb-data.h"
#include "hb-config.h"

typedef int32_t hb_proc_char_t;

#define HB_PROC_CHAR_EOD -1 // End Of Data
#define HB_PROC_CHAR_SIZE sizeof(hb_proc_char_t)

typedef bool hb_proc_predicate_t(hb_proc_char_t);

// Reader and writer callbacks. The last parameter is a pointer to an error
// message. If the last parameter is not NULL, it is assumed an error occurred.
// The error message WILL BE free'd by the callee automatically, so ensure the
// message was created using malloc or strdup, and is not free'd by the function
// or anything else afterwards.
typedef hb_proc_char_t hb_proc_reader_t(void*, char**);
typedef void hb_proc_writer_t(void*, hb_proc_char_t, char**);

#define HB_PROC_MEMORY_CREATE(name)                                            \
	hb_proc_list_memory_instance_add_right_and_return(                     \
		config->memory_instances, name##_create());                    \
	hb_proc_list_memory_destructor_add_right(                              \
		config->memory_destructors,                                    \
		(hb_proc_memory_destructor_t*) &name##_destroy);

NH_LIST(hb_proc_list_memory_instance, void*, sizeof(void*), void*, NULL);
void* hb_proc_list_memory_instance_add_right_and_return(
	hb_proc_list_memory_instance*, void*);

typedef void hb_proc_memory_destructor_t(void*);
NH_LIST(hb_proc_list_memory_destructor, hb_proc_memory_destructor_t*,
	sizeof(hb_proc_memory_destructor_t*), hb_proc_memory_destructor_t*,
	NULL);

#define HB_PROC_ERROR_MESSAGE_SIZE 1024
typedef struct {
	hb_error_t code;
	char* message;
} hb_proc_result_t;

typedef struct {
	char* name;
	jmp_buf start;

	hb_proc_list_memory_instance* memory_instances;
	hb_proc_list_memory_destructor* memory_destructors;

	void* input;
	hb_proc_reader_t* reader;
	bool EOI;

	int line;
	int column;
	bool CR;

	void* output;
	hb_proc_writer_t* writer;
	nh_list_ucp* buffer;
	bool mask;
	nh_list_ucp* redirect;

	hb_config_t config;
} hb_proc_t;

hb_proc_t* hb_proc_create_blank(char* name);
void hb_proc_result_destroy(hb_proc_result_t* result);

hb_proc_result_t* hb_proc_start(hb_proc_t* proc);
void _hb_proc_error(hb_proc_t* proc, hb_error_t code, char const* format, ...);
