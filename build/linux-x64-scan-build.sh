#!/bin/bash

SCAN_BUILD=scan-build-15
BUILD_DIR=linux-x64-scan-build
$SCAN_BUILD cmake -S .. -B $BUILD_DIR -DCMAKE_BUILD_TYPE=Debugs #-GNinja
$SCAN_BUILD cmake --build $BUILD_DIR -j4
