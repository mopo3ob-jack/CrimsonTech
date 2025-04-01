cmake_minimum_required(VERSION 3.12)

project(
	CrimsonTech VERSION 1.0.0
	DESCRIPTION "OpenGL renderer"
	HOMEPAGE_URL "https://github.com/mopo3ob-jack/CrimsonTech"
)

include(GNUInstallDirs)
include(ExternalProject)
include(FindOpenGL)

set(INSTALL_LOCATION ${CMAKE_BINARY_DIR}/deps)

ExternalProject_Add(
	mstd-download
	GIT_REPOSITORY https://github.com/mopo3ob-jack/mstd.git
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
	UPDATE_DISCONNECTED TRUE
)

ExternalProject_Add(
	glfw-download
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
	UPDATE_DISCONNECTED TRUE
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "inc" "*.hpp" "*.h")

add_library(crimson)
add_subdirectory(src/glad)
add_subdirectory(src/crimson/renderer.d)
target_include_directories(crimson PUBLIC inc/ ${INSTALL_LOCATION}/include)
target_link_directories(crimson PUBLIC ${INSTALL_LOCATION}/include)
target_link_libraries(crimson PRIVATE glfw OpenGL::GL OpenGL::GLU)
set_target_properties(crimson PROPERTIES CXX_STANDARD 20)

set(CRIMSON_BUILD_TESTS TRUE CACHE BOOL "Build tiling test programs")
if (${CRIMSON_BUILD_TESTS})
	add_subdirectory("tests/cube")
endif()
