#include <errno.h>
#include <fcntl.h>
#include <hb/cfg.h>
#include <hb/hyperbuild.h>
#include <hb/proc.h>
#include <hb/rule.h>
#include <hb/rune.h>
#include <hb/unit.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/unistd.h>

void hyperbuild_init(void)
{
	hb_rule_init();
}

// Rate to read from file, set to 4 KiB.
#define READ_RATE 4096
// Rate to resize buffer containing file contents, set to 768 KiB.
#define GROWTH_RATE 786432

static void _read_file(char const* file, hb_rune** out, size_t* out_len)
{
	int fd = -1;
	bool success = false;
	hb_rune* output = NULL;

	// Open file.
	fd = open(file, O_RDONLY);
	if (fd < 0) {
		// Failed to open file.
		goto finally;
	}

	// Get file size.
	struct stat stats;
	if (fstat(fd, &stats) != 0) {
		// Failed to get file size.
		goto finally;
	}
	off_t size = stats.st_size;

	// Allocate memory for buffer.
	output = malloc((size + 1) * sizeof(hb_rune));
	size_t output_capacity = size;
	size_t output_next = 0;
	// Read into buffer.
	while (true) {
		// Check if there's enough room to read READ_RATE and reallocate
		// if necessary.
		if (output_next + READ_RATE >= output_capacity) {
			output_capacity += GROWTH_RATE;
			// Make room for terminator.
			hb_rune* new_output =
				realloc(output, output_capacity + 1);
			if (new_output == NULL) {
				// Failed to reallocate memory.
				goto finally;
			}
			output = new_output;
		}

		// Attempt to read READ_RATE.
		ssize_t read_amount = read(fd, output + output_next, READ_RATE);
		if (read_amount < 0) {
			// Failed to read.
			goto finally;
		}

		if (read_amount == 0) {
			// Reached EOF.
			break;
		}
		output_next += read_amount;
	}

	output[output_next] = '\xFF';
	*out_len = output_next;
	success = true;

finally:
	if (fd >= 0) {
		// File descriptor is valid (success or not), close it.
		if (close(fd) != 0) {
			// Failed to close file descriptor.
			success = false;
		}
	}
	if (!success && output != NULL) {
		// Failed to read file, free memory and return NULL.
		free(output);
		output = NULL;
	}
	*out = output;
}

static void _set_file_read_error(hb_proc_result* result)
{
	char* msg = malloc(HB_PROC_ERROR_CUSTOM_SIZE * sizeof(char));
	snprintf(msg, HB_PROC_ERROR_CUSTOM_SIZE,
		 "Failed to read file with system error %d", errno);
	result->code = HB_ERR_IO_FREAD_FAIL;
	result->msg = msg;
	result->pos = 0;
}

hb_rune* hyperbuild_from_file(char const* file, hb_cfg* cfg,
			      hb_proc_result* result)
{
	hb_rune* input;
	size_t input_size;
	_read_file(file, &input, &input_size);
	if (input == NULL) {
		_set_file_read_error(result);
	}

	hyperbuild(input, input_size, input, cfg, result);
	return input;
}

void hyperbuild_from_file_custom_output(char const* file, hb_rune* output,
					hb_cfg* cfg, hb_proc_result* result)
{
	hb_rune* input;
	size_t input_size;
	_read_file(file, &input, &input_size);
	if (input == NULL) {
		_set_file_read_error(result);
	}

	hyperbuild(input, input_size, output, cfg, result);
	free(input);
}

hb_rune* hyperbuild_from_input(hb_rune* input, size_t input_size, hb_cfg* cfg,
			       hb_proc_result* result)
{
	hb_rune* output = malloc((input_size + 1) * sizeof(hb_rune));
	// This function will ensure output is null terminated.
	hyperbuild(input, input_size, output, cfg, result);
	return output;
}

void hyperbuild_in_place(hb_rune* input, size_t input_size, hb_cfg* cfg,
			 hb_proc_result* result)
{
	hyperbuild(input, input_size, input, cfg, result);
}

void hyperbuild(hb_rune* input, size_t input_size, hb_rune* output, hb_cfg* cfg,
		hb_proc_result* result)
{
	input[input_size] = '\xFF';

	hb_proc proc = {
		.cfg = cfg,
		.src = input,
		.src_len = input_size,
		.src_next = 0,
		.out = output,
		.out_next = 0,
		.result = result,
	};

	if (!setjmp(proc.start)) {
		hb_unit_content_html(&proc, NULL);
		// No errors occurred.
		result->code = HB_ERR_OK;
		result->pos = proc.out_next;
		result->msg = NULL;

		// Null terminate output.
		output[proc.out_next] = '\0';
	} else {
		// An error occurred.
	}
}
