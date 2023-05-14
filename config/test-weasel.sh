#!/bin/bash

build/tools/weaselc/weaselc $1 "$2.o" || exit 1
clang++ "$2.o" -o $2 \
    '/Volumes/SideJob/Open Source/weasel/build/libparallel/libWeaselParallel.a' \
    || exit 1
echo -e "\n"

$2 || echo -e "\n" || exit 0
