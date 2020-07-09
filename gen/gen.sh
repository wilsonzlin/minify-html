#!/usr/bin/env bash

set -e

pushd "$(dirname "$0")"

node node_modules/ts-node/dist/bin.js attrs.ts
node node_modules/ts-node/dist/bin.js codepoints.ts
node node_modules/ts-node/dist/bin.js entities.ts

popd
