#pragma once

#import <stdint.h>

// EOF represents the end of an input buffer, and is used for some functions that return characters.
// It must be a value that would never appear in any valid UTF-8 byte sequence.
#define HB_EOF -1

// This version of hyperbuild is designed for ASCII and works with UTF-8 (with minor exceptions), so each character is one byte.
// Use char to maximise compatibility with external and standard libraries.
typedef char hb_rune;
// When either a character or EOF needs to be returned, a character will be represented by a valid hb_rune value and EOF will be represented by HB_EOF.
// In this case, since HB_EOF fits within the valid values of hb_rune, no separate type is needed. A separate type is still used to symbolically represent possible HB_EOF return values.
typedef char hb_eof_rune;
