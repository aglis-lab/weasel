#!/bin/bash

if [ ! -d "build" ]
then
    mkdir build
    cd build
    cmake ..
else
    cd build
fi

if [ $# -eq 0 ]
then
    cmake --build . -j 4
else
    cmake --build . -j 4 --target $1
fi
