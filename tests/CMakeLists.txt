

set(CMAKE_CXX_STANDARD 11)

add_executable (test_mqtt_client_object test_mqtt_client_object.cpp)


target_include_directories (test_mqtt_client_object PRIVATE
    #${TEST_SOURCE_DIR}/modules/
)

include (../cmake/CodeCoverage.cmake)
append_coverage_compiler_flags()

setup_target_for_coverage_lcov(
           NAME coverage
           EXECUTABLE SoundfruuzeTests
           BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/"
           EXCLUDE "/Applications"
           )