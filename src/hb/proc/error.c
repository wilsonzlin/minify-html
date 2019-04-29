#include <hb/proc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static void hb_proc_error_setandjmp(hb_proc* proc, hb_err code, size_t pos,
				    char* msg)
{
	proc->result->code = code;
	proc->result->pos = pos;
	proc->result->msg = msg;
	longjmp(proc->start, 1);
}

void hb_proc_error_pos_len(hb_proc* proc, hb_err code, size_t pos,
			   char const* msg, size_t msg_len)
{
	char* dup = malloc((msg_len + 1) * sizeof(char));
	memcpy(dup, msg, msg_len);
	dup[msg_len] = '\0';
	hb_proc_error_setandjmp(proc, code, pos, dup);
}

void hb_proc_error_custom_pos(hb_proc* proc, hb_err code, size_t pos,
			      char const* format, ...)
{
	va_list args;
	va_start(args, format);

	char* msg = calloc(HB_PROC_ERROR_CUSTOM_SIZE, sizeof(char));
	vsnprintf(msg, HB_PROC_ERROR_CUSTOM_SIZE - 1, format, args);

	va_end(args);

	hb_proc_error_setandjmp(proc, code, pos, msg);
}
