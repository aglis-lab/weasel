#!/bin/bash

# PRE-DEFINE VARIABLES
LLVM_PACKAGE="llvm@13"
LLVM_PATH=/opt/homebrew/opt/$LLVM_PACKAGE/lib/cmake/llvm/
LLVM_BIN_PATH=/opt/homebrew/opt/$LLVM_PACKAGE

# GITHUB LLVM SPIRV
CLSPV_SOURCE=spirv-clspv
CLSPV_URL=https://github.com/google/clspv.git

if [ ! -d "$CLSPV_SOURCE" ]
then
    git clone $CLSPV_URL $CLSPV_SOURCE
fi

if [ ! -d "$CLSPV_SOURCE/build" ]
then
    mkdir $CLSPV_SOURCE/build
fi

if [ ! -d "$CLSPV_SOURCE/third_party/SPIRV-Headers" ]
then
    python3 $CLSPV_SOURCE/utils/fetch_sources.py
fi

cd $CLSPV_SOURCE/build
cmake -GNinja -DLIBCLC_TARGETS_TO_BUILD="clspv--" -DCLSPV_LLVM_BINARY_DIR=$LLVM_BIN_PATH -DCLSPV_LLVM_SOURCE_DIR=$LLVM_PATH -DEXTERNAL_LLVM=1 ..
ninja
