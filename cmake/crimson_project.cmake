cmake_minimum_required(VERSION 3.12)

function(create_test name)
	project(${name} VERSION 1.0.0)
	file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "inc" "*.hpp" "*.h")

	add_executable(${name})
	target_include_directories(${name} PUBLIC inc/${name})
	target_link_libraries(${name} PUBLIC crimson)
	set_target_properties(${name} PROPERTIES CXX_STANDARD 20)
	add_subdirectory(src/${name})

	set_target_properties(${name} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)

	add_custom_command(
		TARGET ${name} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		${STEAM_LIB}
		${CMAKE_CURRENT_BINARY_DIR}
	)

	message("CURRENT TARGET FILE DIR IS: ${CMAKE_CURRENT_BINARY_DIR}")

	message("CURRENT LIBRARY DIR IS: ${LIBRARY_DIR}")

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
