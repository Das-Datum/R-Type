#!/bin/bash

set -e

BUILD_DIR="build/linux"
EXECUTABLE="main"

echo "Creating Linux build directory..."
mkdir -p "$BUILD_DIR"

echo "Configuring the Linux project with CMake..."
cmake -S . -B "$BUILD_DIR"

echo "Building the Linux project..."
cmake --build "$BUILD_DIR" -- -j$(nproc)

echo "Build succeeded. The executable is in $BUILD_DIR/$EXECUTABLE."
