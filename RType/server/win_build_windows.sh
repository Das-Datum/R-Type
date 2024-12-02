#!/bin/bash

BUILD_DIR="build/windows"
EXECUTABLE="server.exe"

echo "Creating Windows build directory..."
mkdir -p "$BUILD_DIR"

echo "Configuring the Windows project with CMake..."
cd "$BUILD_DIR" || exit 1
cmake -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ../.. || { echo "CMake configuration failed!"; exit 1; }

echo "Building the Windows project..."
mingw32-make || { echo "Build failed!"; exit 1; }

echo "Build succeeded. The executable is in $BUILD_DIR/$EXECUTABLE."
