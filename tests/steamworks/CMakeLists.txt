include(crimson_project)

create_test(steamworks)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_link_libraries(steamworks PUBLIC ${steam})
