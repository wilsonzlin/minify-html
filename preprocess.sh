#!/usr/bin/env bash

pushd "$(dirname "$0")" > /dev/null

rm -rf tmp
mkdir -p tmp

for f in $(find src/main/c -type f -name '*.c'); do
  content="$(cat "$f")"
  no_prefix="$(echo "$f" | sed -e 's%^src/main/c/%%')"
  guard="$(echo "$no_prefix" | sed -e 's%\.c$%%' | tr '[a-z]' '[A-Z]' | tr '[/.\-]' '_')"
  util_include="$([[ "$f" != src/main/c/util/* ]] && echo "#include <util/__main__.c>")"
  rule_include="$([[ "$f" != src/main/c/util/* ]] && [[ "$f" != src/main/c/rule/* ]] && echo "#include <rule/__main__.c>")"
  IFS='' read -r -d '' wrapped << EOF
#ifndef _HDR_HYPBERUILD_${guard}
#define _HDR_HYPBERUILD_${guard}

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <nicehash/set/int32.h>
#include <nicehash/set/int64.h>
#include <nicehash/set/str.h>
#include <nicehash/map/str.h>

${util_include}
${rule_include}

${content}

#endif // _HDR_HYPBERUILD_${guard}
EOF
  dest="tmp/$no_prefix"
  mkdir -p "$(dirname "$dest")"
  echo "${wrapped}" > "tmp/$no_prefix"
done

popd > /dev/null

exit 0
