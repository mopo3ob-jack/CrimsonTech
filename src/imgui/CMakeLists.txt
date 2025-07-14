target_sources(
	crimson PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/imgui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui_demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui_draw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui_tables.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui_widgets.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_opengl3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_glfw.cpp
)
