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
pip3 install -r requirements.txt

echo "Creating build directory"
mkdir -p build

if ! [[ -z $(ls build) ]]; then
    echo "WARNING: Build directory not empty, deleting its content."
    rm -r build/*
fi

echo "Configuring cmake project"

if [[ $(uname) == "Linux"  ]];then
    echo "Building for Linux."
    cmake -S . -B build -DCMAKE_CXX_COMPILER=g++ > cmake_log.txt
    else
    echo "Building for Windows."
    cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++ > cmake_log.txt
fi


if [[ -z $(cat cmake_log.txt | grep 'Generating done' && cat cmake_log.txt | grep 'Configuring done') ]]; then
    echo "ERROR: Cmake configuration failed!!! Check 'cmake_log.txt'"
    else
    echo "Cmake configured successfully. Installing executable."
    rm cmake_log.txt
fi

avail_cpus=$(nproc --all)
echo Using $avail_cpus cores
cd build
make -j$avail_cpus > make_log.txt

if [[ -z $(cat make_log.txt | grep '\[100\%\] Built target 3D-fluid-simulator') ]]; then
    echo "ERROR: Building executable failed. Check './build/make_log.txt' and terminal output."
fi

exec_path=$(find . -executable -type f | grep -E '3D-fluid-simulator(\.exe)?$')

echo $exec_path

if [[ -z $exec_path ]]; then
    echo "ERROR: Unknown error, simulator executable not found."
    else
    echo "Executable is present at: $exec_path"
fi
