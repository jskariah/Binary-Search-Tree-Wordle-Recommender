#!/usr/bin/env bash

cd "$(dirname "$0")"
zip -r proj4-bst.zip . \
  -x "*.o" \
  -x "*.zip" \
  -x "*.dSYM/*" \
  -x ".DS_Store" \
  -x "bstset_tests" \
  -x "bstset_main" \
  -x "wordle_main" \
  -x "wordle_tests" \
  -x "wordle_server"
  -x "$(basename $0)"
