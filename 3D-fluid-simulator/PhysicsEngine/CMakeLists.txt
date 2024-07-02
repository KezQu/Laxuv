cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project (PhysicsEngine)

set(PENGINE_INC_DIR ${PROJECT_SOURCE_DIR}/include)

file(GLOB PENGINE_SRC_FILES source/*.cpp)

add_library(PhysicsEngine ${PENGINE_SRC_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_link_libraries(PhysicsEngine PUBLIC glm)

target_include_directories(PhysicsEngine PUBLIC ${PENGINE_INC_DIR})