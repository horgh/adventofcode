#!/bin/bash
set -e
find . -name '*.[ch]' \! -name *siphash.c -exec clang-format-4.0 -i {} \;
