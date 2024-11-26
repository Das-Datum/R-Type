#!/bin/bash

set -e

BUILD_DIR="build/windows"
EXECUTABLE="main.exe"

if ! dpkg -s mingw-w64 >/dev/null 2>&1; then
  echo "mingw-w64 is not installed. Installing..."
  sudo apt-get update
  sudo apt-get install -y mingw-w64
fi

export PATH=/usr/bin/x86_64-w64-mingw32/bin:$PATH

echo "Creating Windows build directory..."
mkdir -p "$BUILD_DIR"

echo "Configuring the Windows project with CMake..."
cmake -S . -B "$BUILD_DIR" -DCMAKE_SYSTEM_NAME=Windows

echo "Building the Windows project..."
cmake --build "$BUILD_DIR" -- -j$(nproc)

echo "Build succeeded. The executable is in $BUILD_DIR/$EXECUTABLE."
