#!/usr/bin/env bash

set -e

pushd "$(dirname "$0")"

# Generate common::gen::* code.
pushd gen
npm i
bash gen.sh
popd

popd
