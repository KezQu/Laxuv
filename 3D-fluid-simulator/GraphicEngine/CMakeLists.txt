cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project (GraphicEngine)

set(GENGINE_SRC_FILES "")
set(GENGINE_INC_DIR "")

file(GLOB_RECURSE GENGINE_SRC_DIRS LIST_DIRECTORIES true *)

foreach(FILE ${GENGINE_SRC_DIRS})
	if(IS_DIRECTORY ${FILE})
		file(GLOB SRC_FILES ${FILE}/*.cpp)
		set(GENGINE_SRC_FILES ${GENGINE_SRC_FILES} ${SRC_FILES})
		set(GENGINE_INC_DIR ${GENGINE_INC_DIR} ${FILE})
	endif()
endforeach()

add_library(GraphicEngine ${GENGINE_SRC_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

find_package(Threads REQUIRED)
target_link_libraries(GraphicEngine PUBLIC glew)
target_link_libraries(GraphicEngine PUBLIC glm)
target_link_libraries(GraphicEngine PUBLIC glfw)
target_link_libraries(GraphicEngine PUBLIC imgui)
target_link_libraries(GraphicEngine PUBLIC pthread)

target_include_directories(GraphicEngine PUBLIC ${GENGINE_INC_DIR})
