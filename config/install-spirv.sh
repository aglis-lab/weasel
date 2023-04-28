#!/bin/bash

# SPIRV LLVM TRANSLATOR JUST SUPPORT OPENCL LANGUAGE #

# PRE-DEFINE VARIABLES
LLVM_PACKAGE="llvm@13"
LLVM_PATH=/opt/homebrew/opt/$LLVM_PACKAGE/lib/cmake/llvm/

# GITHUB LLVM SPIRV
LLVM_SPIRV_BRANCH=llvm_release_130
LLVM_SPIRV_SOURCE=spirv
LLVM_SPIRV_URL=https://github.com/KhronosGroup/SPIRV-LLVM-Translator.git

if [ ! -d "$LLVM_SPIRV_SOURCE" ]
then
    git clone -b $LLVM_SPIRV_BRANCH $LLVM_SPIRV_URL $LLVM_SPIRV_SOURCE
fi

if [ ! -d "$LLVM_SPIRV_SOURCE/build" ]
then
    mkdir $LLVM_SPIRV_SOURCE/build
fi

cd $LLVM_SPIRV_SOURCE/build
cmake .. -DLLVM_DIR=$LLVM_PATH
# cmake .. -DLLVM_DIR=$LLVM_PATH -DCMAKE_GENERATOR:INTERNAL=Ninja -DLLVM_SPIRV_BUILD_EXTERNAL=YES
# cmake --build .
make llvm-spirv -j 4
# cd ../..
# cp -r $LLVM_SPIRV_SOURCE/include $LLVM_SPIRV_SOURCE/build
