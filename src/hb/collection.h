#pragma once

#include <stdint.h>
#include <nicehash/set-int32.h>
#include <nicehash/set-str.h>
#include <nicehash/map-str.h>

NH_MAP_STR(int32, int32_t, int32_t, -1);
NH_MAP_STR(set_str, nh_set_str*, nh_set_str*, NULL);
