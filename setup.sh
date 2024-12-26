#!/bin/bash

echo "Installing 3D-fluid-simulator"

if ! [[ -f CMakeLists.txt ]]; then
    echo "Not in the root directory of the project, exiting..."
    exit 1
fi

echo "Checking needed dependencies"
if [[ -z $(g++ --version | grep -E 'g++.*1[0-9]') ]]; then
    echo "g++ version 10 or higher is required, exiting..."
    exit 1
    else
    echo "Correct g++ version found, continuing..."
fi

if [[ -z $(python3 --version) ]]; then
    echo "python3 is required, exiting..."
    exit 1
    else
    echo "Python3 found, continuing..."
fi

echo "Installing needed python modules"
pip3 install -r requirements.txt > pip_log.txt 2>&1

if [[ -z $(pip list | grep matplotlib) ]]; then
    echo "ERROR: Installing python modules failed. Check 'pip_log.txt'"
    exit 1
    else
    echo "Python modules installed successfully, continuing..."
    rm pip_log.txt
fi

echo "Creating build directory"
mkdir -p build

if ! [[ -z $(ls build) ]]; then
    echo "WARNING: Build directory not empty, deleting its content."
    rm -r build/*
fi

echo "Configuring CMake project"

if [[ $(uname) == "Linux"  ]];then
    echo "Building for Linux."
    cmake -S . -B build -DCMAKE_CXX_COMPILER=g++ > cmake_log.txt 2>&1
    else
    echo "Building for Windows."
    cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++ > cmake_log.txt 2>&1
fi

if [[ -z $(cat cmake_log.txt | grep 'Configuring done') ]]; then
    echo "ERROR: CMake configuration failed!!! Check 'cmake_log.txt'"
    exit 1
    else
    echo "Cmake configured successfully. generating..."
fi

if [[ -z $(cat cmake_log.txt | grep 'Generating done') ]]; then
    echo "ERROR: CMake generation failed!!! Check 'cmake_log.txt'"
    exit 1
    else
    echo "Cmake generated done. Installing executable..."
    rm cmake_log.txt
fi

avail_cpus=$(nproc --all)
echo Using $avail_cpus cores
cd build
make -j$avail_cpus > make_log.txt 2>&1

if [[ -z $(cat make_log.txt | grep '\[100\%\] Built target 3D-fluid-simulator') ]]; then
    echo "ERROR: Building executable failed. Check './build/make_log.txt'."
    exit 1
fi

cd ..
exec_path=$(find . -executable -type f | grep -E '3D-fluid-simulator(\.exe)?$')

if [[ -z $exec_path ]]; then
    echo "ERROR: Unknown error, simulator executable not found."
    else
    echo "Executable is present at: $exec_path"
fi
