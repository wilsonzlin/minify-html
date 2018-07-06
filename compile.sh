#!/usr/bin/env bash

set -e

pushd "$(dirname "$0")" > /dev/null

mkdir -p out

if [ "$1" = "--debug" ]; then
  gcc --std=c11 -Wall -Wextra -Werror -O0 -g -o out/hyperbuild src/main/c/main.c
else
  gcc --std=c11 -Wall -Wextra -Werror -O2 -o out/hyperbuild src/main/c/main.c
fi

popd > /dev/null

exit 0
