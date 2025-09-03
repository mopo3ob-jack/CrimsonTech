cmake_minimum_required(VERSION 3.12)

function(create_test name)
	project(${name} VERSION 1.0.0)
	file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "inc" "*.hpp" "*.h")

	add_executable(${PROJECT_NAME})
	target_include_directories(${PROJECT_NAME} PUBLIC inc/${PROJECT_NAME})
	target_link_libraries(${PROJECT_NAME} PUBLIC crimson)
	set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
	add_subdirectory(src/${PROJECT_NAME})
endfunction()

function(add_resource path)
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/resources/${path}
		${CMAKE_CURRENT_BINARY_DIR}/resources/${path}
		COPYONLY
	)
endfunction()
