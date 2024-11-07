cmake_minimum_required(VERSION 3.0...3.20 FATAL_ERROR)

project (implot)

set(IMPLOT_INC_DIR ${PROJECT_SOURCE_DIR}/)

file (GLOB IMPLOT_SRC_FILES *.cpp)

add_library(implot ${IMPLOT_SRC_FILES})

target_link_libraries(implot PUBLIC imgui)

target_include_directories(implot PUBLIC ${IMPLOT_INC_DIR})