cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.11)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

project(mqtt-object-common)

set(SRC 
${CMAKE_CURRENT_SOURCE_DIR}/src/MQTTClient.hpp
${CMAKE_CURRENT_SOURCE_DIR}/src/MQTTClient.cpp

)

set(MQTT_INCLUDES
${CMAKE_CURRENT_SOURCE_DIR}/src/

${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/paho.mqtt.c/src/
${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/paho.mqtt.cpp/src/

)

###

set(PAHO_BUILD_STATIC ON CACHE STRING "PAHO static")
set(PAHO_BUILD_SHARED OFF CACHE STRING "PAHO shared")
set(PAHO_BUILD_DOCUMENTATION FALSE  CACHE STRING "PAHO docs")
set(PAHO_BUILD_SAMPLES FALSE  CACHE STRING "PAHO samples")
set(PAHO_ENABLE_TESTING OFF CACHE STRING "PAHO tests")

set(PAHO_WITH_SSL OFF CACHE STRING "PAHO ssl")

add_subdirectory(../../third_party/paho.mqtt.c "mqtt-c")

set(PAHO_MQTT_C_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/paho.mqtt.c/src/)

if (WIN32)
    set(PAHO_LIB_NAME libpaho-mqtt3a-static)
else()
    set(PAHO_LIB_NAME libpaho-mqtt3a)
endif()

set(PAHO_MQTT_C_LIBRARIES
    "${CMAKE_BINARY_DIR}/modules/mqtt-object-common/mqtt-c/src/${PAHO_LIB_NAME}.a"
)

message(${PAHO_MQTT_C_LIBRARIES})

add_subdirectory(../../third_party/paho.mqtt.cpp "mqtt-cxx")

###

add_library(mqtt-object STATIC)
target_sources(mqtt-object PRIVATE ${SRC})
target_include_directories(mqtt-object PRIVATE ${MQTT_INCLUDES})

target_link_libraries(mqtt-object
 paho-mqtt3a-static
 paho-mqttpp3-static
)

target_compile_features(mqtt-object PRIVATE cxx_std_14)

add_dependencies(mqtt-object
    paho-mqtt3a-static
    )

target_link_directories(mqtt-object PRIVATE
   ${CMAKE_BINARY_DIR}/third_party/paho.mqtt.c/src/
)


