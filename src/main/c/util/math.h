#ifndef _HDR_HYPERBUILD_UTIL_MATH
#define _HDR_HYPERBUILD_UTIL_MATH

#define hbu_max(a, b) \
  ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#endif // _HDR_HYPERBUILD_UTIL_MATH
