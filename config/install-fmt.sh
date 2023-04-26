#!/bin/bash

# GITHUB GLOG
ROOT_DIR=third-party
PACKAGE_URL=https://github.com/fmtlib/fmt.git
PACKGE_NAME=fmt
PACKGE_DIR=$ROOT_DIR/$PACKGE_NAME

if [ ! -d "$PACKGE_DIR" ]
then
    cd $ROOT_DIR
    git clone $PACKAGE_URL $PACKGE_NAME
    cd ..
fi

if [ ! -d "$PACKGE_DIR/build" ]
then
    cd $ROOT_DIR
    mkdir $PACKGE_NAME/build
    cd ..
fi

cd $PACKGE_DIR
cmake -S . -B build -G "Unix Makefiles"
cmake --build build --config Release
cmake --build build --target install
