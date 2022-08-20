#!/bin/bash

echo "Build Objects"
build/tools/weaselc/weaselc $1 "$2.o" || exit 1

echo "Build Executable"
clang++ "$2.o" -o $2  || exit 1

echo -e "Run Executable\n\n"
$2 || exit 1
