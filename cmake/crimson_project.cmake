cmake_minimum_required(VERSION 3.12)

function(create_test name)
	project(${name} VERSION 1.0.0)
	file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "inc" "*.hpp" "*.h")

	add_executable(${PROJECT_NAME})
	target_include_directories(${PROJECT_NAME} PUBLIC inc/${PROJECT_NAME})
	target_link_libraries(${PROJECT_NAME} PUBLIC crimson)
	set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
	add_subdirectory(src/${PROJECT_NAME})

	set_target_properties(${name} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)

	add_custom_command(
		TARGET ${name} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${LIBRARY_DIR}
		$<TARGET_FILE_DIR:${name}>
	)

	set(appid ${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt)
	if (EXISTS ${appid})
		configure_file(
			${appid}
			${CMAKE_CURRENT_BINARY_DIR}/steam_appid.txt
			COPYONLY
		)
	else()
		message(WARNING "No steam_appid.txt present")
	endif()
endfunction()

function(add_resource path)
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/resources/${path}
		${CMAKE_CURRENT_BINARY_DIR}/resources/${path}
		COPYONLY
	)
endfunction()
