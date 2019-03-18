#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <hb/rune.h>
#include <hb/cfg.h>
#include <hb/err.h>
#include <setjmp.h>

typedef struct {
  hb_err code;
  char* msg;
} hb_proc_result;

typedef struct {
  hb_cfg* cfg;
  char* name;
  jmp_buf start;

  hb_rune* src;
  size_t src_len;
  size_t src_next;
  bool eof;

  int line;
  int col;
  bool cr;

  hb_rune* out;
  size_t out_len;
  size_t out_next;
  hb_proc_result result;
} hb_proc;

typedef struct {
    size_t start;
    size_t end;
} hb_proc_view;

bool hb_proc_view_invalid(hb_proc_view view);

#define hb_proc_matches(proc, match) hb_proc_matches_len(proc, match, sizeof(match))
size_t hb_proc_matches_len(hb_proc* proc, char const* match, size_t match_len);
hb_rune hb_proc_peek_eof(hb_proc* proc);
