#!/bin/bash
set -e

BUILD_DIR=build
BUILD_TYPE=Debug
CLANG=$(command -v clang)
CLANGXX=$(command -v clang++)
CMAKE_C_COMPILER=clang
CMAKE_CXX_COMPILER=clang++

# Check for clang and clang++
if [ -z "$CLANG" ] || [ -z "$CLANGXX" ]; then
  echo "clang or clang++ not found, using default gnu/gcc."
  CMAKE_C_COMPILER=cc
  CMAKE_CXX_COMPILER=c++
fi

if [ -d "$BUILD_DIR" ]; then
    echo "Removing existing build directory..."
    rm -rf "$BUILD_DIR"
fi

echo "Creating build directory..."
mkdir "$BUILD_DIR"

cd "$BUILD_DIR"

echo "Generating project with CMake..."
cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DTESTS=OFF -DUSE_Z3=OFF \
-DCMAKE_C_COMPILER="$CMAKE_C_COMPILER" -DCMAKE_CXX_COMPILER="$CMAKE_CXX_COMPILER"

echo "Building project..."
make -j8

echo "Build completed."

