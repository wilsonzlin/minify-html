#!/usr/bin/env bash

shopt -s globstar

pushd "$(dirname "$0")" > /dev/null

tmp_out="$(mktemp)"
fail_count=0
for test_file in ./test/**/*.c; do
    gcc --std=c11 -Wall -Wextra -Werror -O0 -g -Ilib -Isrc -Itest -o "$tmp_out" "$test_file" lib/**/*.c src/**/*.c || exit 1
    "$tmp_out"
    res="$?"
    if [[ $res != 0 ]]; then
        fail_count=$((fail_count + 1))
    fi;
done

if [[ $fail_count == 0 ]]; then
    echo "All tests passed successfully"
fi

popd > /dev/null
