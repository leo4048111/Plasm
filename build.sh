#!/bin/bash
set -e

BUILD_DIR=build

if [ -d "$BUILD_DIR" ]; then
    echo "Removing existing build directory..."
    rm -rf "$BUILD_DIR"
fi

echo "Creating build directory..."
mkdir "$BUILD_DIR"

cd "$BUILD_DIR"

echo "Generating project with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Debug -DTESTS=OFF

echo "Building project..."
make -j8

echo "Build completed."

