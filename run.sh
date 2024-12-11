#!/bin/bash

if [ ! -d "./build" ]; then
    mkdir ./build
fi

cd ./build

if [ "$1" != "--run" ]; then
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
fi

if [ "$1" == "--server" ]; then
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        cd RType/server/ && ./server
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        cd RType/server/ && ./server
    elif [[ "$OSTYPE" == "cygwin" ]]; then
        cd RType/server/ && ./server.exe
    elif [[ "$OSTYPE" == "msys" ]]; then
        cd RType/server/ && ./server.exe
    elif [[ "$OSTYPE" == "win32" ]]; then
        cd RType/server/ && ./server.exe
    else
        echo "Unsupported OS"
        exit 1
    fi
else
    echo "Starting client..."
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        cd RType/client/ && ./client
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        cd RType/client/ && ./client
    elif [[ "$OSTYPE" == "cygwin" ]]; then
        cd RType/client/ && ./client.exe
    elif [[ "$OSTYPE" == "msys" ]]; then
        cd RType/client/ && ./client.exe
    elif [[ "$OSTYPE" == "win32" ]]; then
        cd RType/client/ && ./client.exe
    else
        echo "Unsupported OS"
        exit 1
    fi
fi
