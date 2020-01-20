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
  sed -i 's%^hyperbuild = .*$%hyperbuild = { path = "../.." }%' native/Cargo.toml
fi
npx neon build --release
mv native/Cargo.toml.orig native/Cargo.toml

if [[ "$OSTYPE" == "linux-gnu" ]]; then
  os_name="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
  os_name="macos"
elif [[ "$OSTYPE" == "cygwin" ]]; then
  os_name="windows"
elif [[ "$OSTYPE" == "msys" ]]; then
  os_name="windows"
elif [[ "$OSTYPE" == "win32" ]]; then
  os_name="windows"
else
  echo "Unknown OS"
  exit 1
fi

node_version=$(node -e 'console.log(process.versions.node.split(".")[0])')

mv native/index.node "dist/$os_name-x86_64-node$node_version.node"

popd
