#!/usr/bin/env bash

set -e
shopt -s globstar

pushd "$(dirname "$0")"

if [ "$1" = "release" ]; then
  rust_build_arg="--release"
  rust_build_dir="release"
else
  rust_build_arg=""
  rust_build_dir="debug"
fi


rm -rf target/java
mkdir -p target/java
javac -d target/java src/main/java/**/*.java

cargo build $rust_build_arg
cp target/rust/$rust_build_dir/libhyperbuild_java.so target/java/in/wilsonl/hyperbuild/.

popd
