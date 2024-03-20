cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project (GraphicEngine)

set(GENGINE_SRC_FILES "")
set(GENGINE_INC_DIR "")

file(GLOB GENGINE_SRC_DIRS [GLOB_DIRECTORIES true] *)

foreach(DIR ${GENGINE_SRC_DIRS})
	if(IS_DIRECTORY ${DIR})
		file(GLOB SRC_FILES ${DIR}/*.cpp)
		set(GENGINE_SRC_FILES ${GENGINE_SRC_FILES} ${SRC_FILES})
		set(GENGINE_INC_DIR ${GENGINE_INC_DIR} ${DIR})
	endif()
endforeach()

add_library(GraphicEngine ${GENGINE_SRC_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_link_libraries(GraphicEngine PUBLIC glm)
target_link_libraries(GraphicEngine PUBLIC UserInterface)
target_link_libraries(GraphicEngine PUBLIC glew)

target_include_directories(GraphicEngine PUBLIC ${GENGINE_INC_DIR})