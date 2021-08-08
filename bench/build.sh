#!/usr/bin/env bash

set -Eeuo pipefail

shopt -s nullglob

pushd runners >/dev/null
for r in *; do
  if [ ! -d "$r" ]; then
    continue
  fi
  echo "Building $r..."
  pushd "$r" >/dev/null
  ./build
  popd >/dev/null
done
popd >/dev/null

echo "All done!"
