# 3D Fluid Simulator

---

The aim for this project is to provide a simple three-dimensional fluid simulator allowing user to test the behavior of the fluid in different flows.
Algorithm used to solve Navier-Stokes equation and the continuity equations is _divergence-free smoothed particle hydrodynamics -- DFSPH_. It's a
variation of a _smoothed particle hydrodynamics_ method invented by the **R. A. Gingold** and **J. J. Monaghan** in 1977. _DFPSH_ extends
the base method by an additional predicate -- the divergence-free velocity vector field.

## Requirements

In order to successfully install the `3D Fluid Simulator` user need to meet following requirements:

- CMake - ver. 3.16
- Make
- Python3 with pip
- Support for OpenGL 4.6
- Compiler with support for C++20

## How to install

Currently application comes with an installation script written in bash currently with support only for the `g++` compiler and for Windows
installation only with `MinGW Makefiles` cmake generator.

#### Using installation script

- For Linux - run `./setup.sh` bash script in the parent directory of the project (possible needed elevation of the permissions to make in an executable),
- For Windows - to run the installation script for Windows it is required to have GNU bash present. If the predicate is met use `sh ./setup.sh`
  command in the parent directory of the project.

#### Without installation script

- Run `pip install -r requirements.txt` in the parent directory of the project. If all packages are installed successfully, then
- Follow CMake procedure to build an CMake project. Possible batch of installation commands:
  - `cmake -S . -B build`- only if the build directory is present,
  - go to build directory and run `make`, or use any other build system used as a default CMake generator.
