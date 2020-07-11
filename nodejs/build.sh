#!/usr/bin/env bash

# Builds native module for current platform and Node.js version.

set -e

pushd "$(dirname "$0")"

npm install

rm -rf dist
npx tsc

npx neon build --release
mv native/index.node "dist/native.node"

popd
