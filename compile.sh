#!/usr/bin/env bash

set -e

pushd "$(dirname "$0")" > /dev/null

mkdir -p out

gcc --std=c99 -Wall -Wextra -Werror -O2 -o out/hyperbuild src/main/c/main.c

popd > /dev/null

exit 0
