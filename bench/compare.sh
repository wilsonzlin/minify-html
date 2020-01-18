#!/usr/bin/env bash

git --no-pager diff --no-index --word-diff=color --word-diff-regex=. "min/html-minifier/$1.html" "min/hyperbuild-nodejs/$1.html" | less
