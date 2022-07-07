#!/bin/bash

# PRE-DEFINE VARIABLES
LLVM_PACKAGE="llvm@13"
LLVM_PATH=/opt/homebrew/opt/$LLVM_PACKAGE/lib/cmake/llvm/
# LLVM SPIRV
LLVM_SPIRV_BRANCH=llvm_release_130
LLVM_SPIRV_SOURCE=llvm-spirv
LLVM_SPIRV_URL=https://github.com/KhronosGroup/SPIRV-LLVM-Translator.git

brew_install() {
    echo "\nInstalling $1"
    if brew list $1 &>/dev/null; then
        echo "${1} is already installed"
    else
        brew install $1 && echo "$1 is installed"
    fi
}

# Install LLVM
# LLVM MAC
brew_install $LLVM_PACKAGE

# SpirV LLVM
git clone -b $LLVM_SPIRV_BRANCH $LLVM_SPIRV_URL $LLVM_SPIRV_SOURCE
mkdir $LLVM_SPIRV_SOURCE/build
cd $LLVM_SPIRV_SOURCE/build
cmake .. -DLLVM_DIR=$LLVM_PATH -DCMAKE_GENERATOR:INTERNAL=Ninja
cmake --build .
cd ../..
cp -r $LLVM_SPIRV_SOURCE/include $LLVM_SPIRV_SOURCE/build
