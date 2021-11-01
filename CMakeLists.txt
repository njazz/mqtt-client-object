project(mqtt-client-object)

add_subdirectory(modules/mqtt-client)
add_subdirectory(modules/mqtt-object-common)

if (WITH_TESTS)
add_subdirectory(tests)
enable_testing()
endif()
