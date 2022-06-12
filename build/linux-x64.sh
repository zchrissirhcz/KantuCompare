#!/bin/bash

BUILD_DIR=linux-x64
mkdir -p $BUILD_DIR
cd $BUILD_DIR
cmake ../.. -DCMAKE_BUILD_TYPE=Debugs #-GNinja
cmake --build .
cd ..