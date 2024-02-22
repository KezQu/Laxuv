
set(GLEW_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(GLEW_INC_DIR ${PROJECT_SOURCE_DIR}/include)

add_library(glew ${GLEW_SRC_DIR}/glew.c)

find_package(OpenGL REQUIRED)

target_link_libraries(glew PUBLIC OpenGL::GL)

target_include_directories(glew PUBLIC ${GLEW_INC_DIR})
target_compile_definitions(glew PUBLIC GLEW_STATIC)
