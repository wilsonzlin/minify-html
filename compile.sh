#!/usr/bin/env bash

pushd "$(dirname "$0")" > /dev/null

set -e

./preprocess.sh

npx tsc

main_js_path="out/main/ts/main.js"
ts_gen="$(cat "$main_js_path")"
echo "module.exports = {}; var exports = module.exports; $ts_gen" > "$main_js_path";

mkdir -p out

cmp_std="--std=c11"
cmp_errors="-Wall -Wextra -Werror"
cmp_inc="-I tmp/ -I lib/"
cmp_optimisation="$([[ "$1" == "--debug" ]] && echo "-O0 -g" || echo "-O3")"
cmp_em_debug="$([[ "$1" == "--debug" ]] && echo "-s ASSERTIONS=2" || echo "-s ASSERTIONS=0")"

gcc $cmp_std $cmp_errors $cmp_optimisation $cmp_inc -o out/hyperbuild "tmp/cli.c"
emcc $cmp_std $cmp_errors $cmp_optimisation $cmp_em_debug $cmp_inc -o out/hyperbuild.em.js "tmp/em.c" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s "EXPORTED_FUNCTIONS=['_hbe_err_code', '_hbe_err_message', '_hbe_err_destroy', '_em_init', '_em_entry']" \
  -s "BINARYEN_TRAP_MODE='clamp'" \
  --post-js "$main_js_path"

rm "$main_js_path"
mv out/main/ts/main.d.ts out/main.d.ts
rm -rf out/main

popd > /dev/null

exit 0
