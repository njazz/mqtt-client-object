

set(CMAKE_CXX_STANDARD 11)

add_executable (test_mqtt_client_object
    test_mqtt_client_object.cpp

    ../modules/mqtt-client/src/mqtt_client.cpp

    test_object.hpp
    MQTTClientMock.cpp

)

target_compile_definitions(test_mqtt_client_object PRIVATE BUILD_TESTS=1)

target_include_directories (test_mqtt_client_object PRIVATE
    #${TEST_SOURCE_DIR}/modules/
	${TEST_SOURCE_DIR}/
        ${CMAKE_SOURCE_DIR}/modules/mqtt-object-common/src/
        ${CMAKE_SOURCE_DIR}/modules/mqtt-client/src/
        ${CMAKE_SOURCE_DIR}/tests/
)

target_link_libraries(test_mqtt_client_object
 paho-mqtt3a-static
 paho-mqttpp3-static
)

include (../cmake/CodeCoverage.cmake)
append_coverage_compiler_flags()

setup_target_for_coverage_lcov(
           NAME coverage
           EXECUTABLE SoundfruuzeTests
           BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/"
           EXCLUDE "/Applications"
           )
