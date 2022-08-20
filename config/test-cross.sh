#!/bin/bash

build/tools/cross/cross $1 "$2.o" || exit 1
