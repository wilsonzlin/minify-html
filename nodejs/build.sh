#!/usr/bin/env bash

set -e

# Builds hyperbuild-nodejs with native module for local testing.
# Built package will only run current platform and Node.js version.

pushd "$(dirname "$0")"

npm install

rm -rf dist
npx tsc

if [ -f native/Cargo.toml.orig ]; then
  echo 'Not altering Node.js Cargo.toml file'
else
  cp native/Cargo.toml native/Cargo.toml.orig
  # Don't use -i as macOS requires '' argument but then Ubuntu will treat as pattern.
  sed 's%^hyperbuild = .*$%hyperbuild = { path = "../.." }%' native/Cargo.toml.orig > native/Cargo.toml
fi
npx neon build --release
mv native/Cargo.toml.orig native/Cargo.toml

binary_name="$(node -e 'console.log([process.platform, process.arch, process.versions.modules].join("__"))')"

mv native/index.node "dist/$binary_name.node"

popd
