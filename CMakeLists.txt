cmake_minimum_required(VERSION 3.12)

project(
	CrimsonTech VERSION 1.0.0
	DESCRIPTION "OpenGL renderer"
	HOMEPAGE_URL "https://github.com/mopo3ob-jack/CrimsonTech"
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(FetchContent)
include(FindOpenGL)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "inc" "*.hpp" "*.h")

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(mstd glfw assimp)

add_library(crimson)
target_compile_definitions(crimson PUBLIC _USE_MATH_DEFINES)
target_include_directories(crimson PUBLIC inc/ ${INSTALL_LOCATION}/include)
target_link_directories(crimson PUBLIC ${INSTALL_LOCATION}/lib)
target_link_libraries(crimson PUBLIC mstd glfw OpenGL::GL OpenGL::GLU assimp ${steam})
set_target_properties(crimson PROPERTIES CXX_STANDARD 20)

add_subdirectory(src/glad)
add_subdirectory(src/imgui)
add_subdirectory(src/stb)
add_subdirectory(src/crimson/renderer.d)
add_subdirectory(src/crimson/file.d)
add_subdirectory(src/crimson/game.d)

if (WIN32)
	set(LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/win64)
else()
	set(LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux64)
endif()

set(CRIMSON_BUILD_TESTS TRUE CACHE BOOL "Build crimson test programs")
if (${CRIMSON_BUILD_TESTS})
	add_subdirectory("tests/bsp")
	add_subdirectory("tests/indirect")
	add_subdirectory("tests/texture")
	add_subdirectory("tests/steamworks")
endif()
