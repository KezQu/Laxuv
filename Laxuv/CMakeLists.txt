# CMakeList.txt : CMake project for Laxuv, include source and define
# project specific logic here.
#
set(INC_DIR ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/include)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/src)

set(GLEW_INC_DIR ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/Dependencies/glew/include)
set(GLFW_INC_DIR ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/Dependencies/glfw/include)

# Add source to this project's executable.
file(GLOB SRC_FILES ${SRC_DIR}/*.cpp)

add_executable (Laxuv ${SRC_FILES})

set_property(TARGET Laxuv PROPERTY CXX_STANDARD 20)

# target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glew)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui)

target_include_directories(${PROJECT_NAME} PUBLIC ${INC_DIR} ${GLEW_INC_DIR} ${GLFW_INC_DIR})


# TODO: Add tests and install targets if needed.
