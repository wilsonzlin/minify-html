#!/usr/bin/env bash

set -e

# Build JAR with native module for current platform.

pushd "$(dirname "$0")"

if [ "$1" = "release" ]; then
  rust_build_arg="--release"
  rust_build_dir="release"
else
  rust_build_arg=""
  rust_build_dir="debug"
fi

if [[ "$OSTYPE" == "linux-gnu" ]]; then
  os_name="linux"
  ext="so"
elif [[ "$OSTYPE" == "darwin"* ]]; then
  os_name="macos"
  ext="dylib"
elif [[ "$OSTYPE" == "cygwin" ]]; then
  os_name="windows"
  ext="dll"
elif [[ "$OSTYPE" == "msys" ]]; then
  os_name="windows"
  ext="dll"
elif [[ "$OSTYPE" == "win32" ]]; then
  os_name="windows"
  ext="dll"
else
  echo "Unknown OS"
  exit 1
fi

cargo build $rust_build_arg
mkdir -p src/main/resources/
cp target/rust/$rust_build_dir/libminify_html_java.$ext src/main/resources/$os_name-x86_64.nativelib

mvn clean package

popd
