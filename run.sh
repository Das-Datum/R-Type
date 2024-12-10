#!/bin/bash

if [ ! -d "./build" ]; then
    mkdir ./build
fi

cd ./build

cmake ..
if [ $? -ne 0 ]; then
    echo "CMake configuration failed"
    exit 1
fi

cmake --build .
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

if [ "$1" == "--server" ]; then
    cd Rtype/server/ && ./server.exe
else
    cd Rtype/client/ && ./client.exe
fi
