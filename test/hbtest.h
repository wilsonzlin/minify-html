#pragma once

#include <stdio.h>

#define expect(cond, msg)                                                      \
	if (!cond)                                                             \
	fprintf(stderr, "Test failed: " msg " [%s %s() line %d]", __FILE__,    \
		__func__, __LINE__)
