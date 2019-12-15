#pragma once

#include <hb/cfg.h>
#include <hb/proc.h>
#include <hb/rune.h>
#include <stddef.h>

/**
 * Initialise internal structures and data used in processing.
 * This function must be called before using any other hyperbuild function.
 */
void hyperbuild_init(void);

/**
 * Read a file and run hyperbuild on the contents. Output will be null
 * terminated if no error occurs.
 *
 * @param file path to the file
 * @param cfg configuration to use
 * @param[out] result where to write any resulting error information
 * @return pointer to a heap-allocated array containing processed output that
 * needs to be freed
 */
hb_rune* hyperbuild_from_file(char const* file, hb_cfg* cfg,
			      hb_proc_result* result);

/**
 * Read a file and run hyperbuild on the contents, writing to {@param output}.
 * Output will be null terminated if no error occurs. WARNING: Does not check if
 * {@param output} is large enough. It should at least match the size of the
 * file.
 *
 * @param file path to the file
 * @param output output array to write to
 * @param cfg configuration to use
 * @param[out] result where to write any resulting error information
 */
void hyperbuild_from_file_custom_output(char const* file, hb_rune* output,
					hb_cfg* cfg, hb_proc_result* result);

/**
 * Run hyperbuild on an input array and write to a heap-allocated array. Output
 * will be null terminated if no error occurs. WARNING: Input must end with
 * '\xFF' or '\0', and {@param input_size} must not include the terminator.
 *
 * @param input input array to process
 * @param cfg configuration to use
 * @param[out] result where to write any resulting error information
 * @return pointer to a heap-allocated array containing processed output that
 * needs to be freed
 */
hb_rune* hyperbuild_from_input(hb_rune* input, size_t input_size, hb_cfg* cfg,
			       hb_proc_result* result);

/**
 * Run hyperbuild in place on an input array. Output will be null terminated if
 * no error occurs. WARNING: Input must end with '\xFF' or '\0', and {@param
 * input_size} must not include the terminator.
 *
 * @param input input array to process
 * @param cfg configuration to use
 * @param[out] result where to write any resulting error information
 */
void hyperbuild_in_place(hb_rune* input, size_t input_size, hb_cfg* cfg,
			 hb_proc_result* result);

/**
 * Run hyperbuild on an input array and write to {@param output}. Output will be
 * null terminated if no error occurs. WARNING: Input must end with '\xFF' or
 * '\0', and {@param input_size} must not include the terminator. WARNING: Does
 * not check if {@param output} is large enough. It should at least match the
 * size of the input.
 *
 * @param input input array to process
 * @param output output array to write to
 * @param cfg configuration to use
 * @param[out] result where to write any resulting error information
 */
void hyperbuild(hb_rune* input, size_t input_size, hb_rune* output, hb_cfg* cfg,
		hb_proc_result* result);
