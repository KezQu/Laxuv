cmake_minimum_required (VERSION 3.0...3.20 FATAL_ERROR)

project (GLEW VERSION 2.1.0)

set(GLEW_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(GLEW_INC_DIR ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(${PROJECT_SOURCE_DIR}/src)