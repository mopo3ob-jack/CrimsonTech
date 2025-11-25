cmake_minimum_required(VERSION 3.12)

include(crimson_project)

create_test(indirect)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_resource(shaders/shader.vert)
add_resource(shaders/shader.frag)
