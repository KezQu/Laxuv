cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 20)

project (UserInterface)

set(UI_INC_DIR ${PROJECT_SOURCE_DIR}/include)

file(GLOB UI_SRC_FILES source/*.cpp)

add_library(UserInterface ${UI_SRC_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME} PUBLIC GraphicEngine)

target_include_directories(UserInterface PUBLIC ${GENGINE_INC_DIR})
target_include_directories(UserInterface PUBLIC ${UI_INC_DIR})