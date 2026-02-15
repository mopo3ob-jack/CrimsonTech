cmake_minimum_required(VERSION 3.21)

add_library(steam SHARED IMPORTED)
if (WIN32)
	set_target_properties(
		steam PROPERTIES
		IMPORTED_LOCATION "lib/win64/steam_api64.dll"
		IMPORTED_IMPLIB "lib/win64/steam_api64.lib"
	)
else()
	set_target_properties(
		steam PROPERTIES
		IMPORTED_LOCATION "lib/linux64/steam_api64.so"
	)
endif()

function(import_steam name)
	if (WIN32)
		add_custom_command(
			TARGET ${name} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${name}> $<TARGET_RUNTIME_DLLS:${name}>
			COMMAND_EXPAND_LISTS
		)
	else()
		set_target_properties(
			steam PROPERTIES
			IMPORTED_LOCATION "lib/linux64/steam_api64.so"
		)
	endif()
endfunction()
