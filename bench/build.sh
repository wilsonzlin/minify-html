#!/usr/bin/env bash

set -e

pushd "$(dirname "$0")"

bash ../nodejs/build.sh

pushd hyperbuild-bench
cargo build --release
popd

popd
