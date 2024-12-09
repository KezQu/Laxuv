set(APP_INC_DIR ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/Application/include)
set(APP_SRC_DIR ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/Application/source)

file(GLOB APP_SRC_FILES ${APP_SRC_DIR}/*.cpp)

add_executable (3D-fluid-simulator ${APP_SRC_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC GraphicEngine)
target_link_libraries(${PROJECT_NAME} PUBLIC UserInterface)

target_include_directories(${PROJECT_NAME} PUBLIC ${APP_INC_DIR})
