#!/bin/bash


set -e

echo "[1/4] Entering build directory..."

mkdir -p build
cd build
rm -rf *
echo "[2/4] Configuring CMake..."
cmake ..

echo "[3/4] Compiling C++ Code..."
cmake --build .

echo "[4/4] Executing gpu-switcher (Requires Sudo)..."

sudo ./gpu-rr-switcher