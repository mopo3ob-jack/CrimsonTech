cmake_minimum_required(VERSION 3.12)

project(cube VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src" "*.cpp" "*.c")

function(add_resource path)
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/resources/${path}
		${CMAKE_CURRENT_BINARY_DIR}/resources/${path}
		COPYONLY
	)
endfunction()

add_resource(shaders/shader.vert)
add_resource(shaders/shader.frag)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC inc/)
target_link_libraries(${PROJECT_NAME} PUBLIC crimson)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
