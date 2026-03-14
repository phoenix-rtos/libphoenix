#!/bin/sh

set -xe

mkdir -p sw-quality/misra-c
find libm/common libm/complex* libm/math* -name  "*.c" | grep -v "fenv.c" > sw-quality/misra-c/files.lnt
wine ~/lint-nt.exe sw-quality/std.lnt sw-quality/misra-c/files.lnt | sed 's#\\#/#g' > sw-quality/misra-c/lint.txt || true
python3 -m sqarg -c sw-quality/sqarg-config.toml
