#!/bin/bash

# PRE-DEFINE VARIABLES
LLVM_PACKAGE="llvm@13"
LLVM_PATH=/opt/homebrew/opt/$LLVM_PACKAGE/lib/cmake/llvm/

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
