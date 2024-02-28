cmake_minimum_required (VERSION 3.0...3.20 FATAL_ERROR)

file (GLOB IMGUI_SRC_FILES *.cpp)
file (GLOB IMGUI_BACKEND_FILES backend/*.cpp)

add_library(imgui ${IMGUI_SRC_FILES} ${IMGUI_BACKEND_FILES})

target_link_libraries(imgui PUBLIC glfw)

target_include_directories(imgui PUBLIC ${IMGUI_INC_DIR} ${IMGUI_BACKEND_INC_DIR})