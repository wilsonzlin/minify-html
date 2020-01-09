#!/usr/bin/env bash

set -e

rm -rf node_modules
HYPERBUILD_NODEJS_SKIP_BIN_DOWNLOAD=1 npm i
pushd hyperbuild-bench
cargo build --release
popd
