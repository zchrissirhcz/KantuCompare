#!/bin/bash

SCAN_BUILD=scan-build-14
BUILD_DIR=linux-x64-scan-build
mkdir -p $BUILD_DIR
cd $BUILD_DIR

$SCAN_BUILD cmake ../.. -DCMAKE_BUILD_TYPE=Debugs #-GNinja
$SCAN_BUILD cmake --build . -j4
cd ..