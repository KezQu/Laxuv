cmake_minimum_required(VERSION 3.0...3.20 FATAL_ERROR)

project (imgui)

set(IMGUI_INC_DIR ${PROJECT_SOURCE_DIR}/include)
set(IMGUI_BACKEND_INC_DIR ${PROJECT_SOURCE_DIR}/src/backend)

add_subdirectory(${PROJECT_SOURCE_DIR}/src)