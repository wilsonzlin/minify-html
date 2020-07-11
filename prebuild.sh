#!/usr/bin/env bash

# Actions to take before initial or release build of main Rust library.

set -e

pushd "$(dirname "$0")"

# Generate crate::gen::* code.
pushd gen
npm i
bash gen.sh
popd

popd
