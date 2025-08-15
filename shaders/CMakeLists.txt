function(add_shader path)
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/${path}
		${CMAKE_CURRENT_BINARY_DIR}/${path}
		COPYONLY
	)
endfunction()

add_shader(lighting.frag)
