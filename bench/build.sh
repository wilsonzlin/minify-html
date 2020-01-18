#!/usr/bin/env bash

set -e

pushd "$(dirname "$0")"

nodejs_cargo_toml="../nodejs/native/Cargo.toml"

if [ -f "$nodejs_cargo_toml.orig" ]; then
  echo 'Not altering Node.js Cargo.toml file'
else
  cp "$nodejs_cargo_toml" "$nodejs_cargo_toml.orig"
fi
sed -i 's%^hyperbuild = .*$%hyperbuild = { path = "../.." }%' "$nodejs_cargo_toml"
HYPERBUILD_NODEJS_SKIP_BIN_DOWNLOAD=1 npm rebuild hyperbuild
mv "$nodejs_cargo_toml.orig" "$nodejs_cargo_toml"
pushd hyperbuild-bench
cargo build --release
popd

popd
