cmake_minimum_required(VERSION 3.12)

project(
	CrimsonTech VERSION 1.0.0
	DESCRIPTION "OpenGL renderer"
	HOMEPAGE_URL "https://github.com/mopo3ob-jack/CrimsonTech"
)

include(GNUInstallDirs)
include(FetchContent)
include(FindOpenGL)

FetchContent_Declare(
	mstd
	GIT_REPOSITORY https://github.com/mopo3ob-jack/mstd.git
)

FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
)

FetchContent_Declare(
	assimp
	GIT_REPOSITORY https://github.com/assimp/assimp.git
)

FetchContent_MakeAvailable(mstd glfw assimp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "inc" "*.hpp" "*.h")

add_library(crimson)
target_compile_definitions(crimson PUBLIC _USE_MATH_DEFINES)
target_include_directories(crimson PUBLIC inc/ ${INSTALL_LOCATION}/include)
target_link_directories(crimson PUBLIC ${INSTALL_LOCATION}/lib)
target_link_libraries(crimson PUBLIC mstd glfw OpenGL::GL OpenGL::GLU assimp)
set_target_properties(crimson PROPERTIES CXX_STANDARD 20)

add_subdirectory(src/glad)
add_subdirectory(src/crimson/renderer.d)
add_subdirectory(src/crimson/file.d)

get_target_property(_glfw_inc glfw INTERFACE_INCLUDE_DIRECTORIES)
message("GLFW include dirs: ${_glfw_inc}")

set(CRIMSON_BUILD_TESTS TRUE CACHE BOOL "Build tiling test programs")
if (${CRIMSON_BUILD_TESTS})
	add_subdirectory("tests/cube")
	add_subdirectory("tests/bsp")
endif()
