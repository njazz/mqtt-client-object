cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.11)
set(CMAKE_OSX_SYSROOT /)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

add_compile_options(-Wall)
# mostly for puredata sources:
#add_compile_options(-Wno-unused-parameter)

if(POLICY CMP0074)
    #policy for <PackageName>_ROOT variables
    cmake_policy(SET CMP0074 NEW)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/compactlib/cmake)

project(mqtt_client)

# pd object
add_library(mqtt_client_pd SHARED src/mqtt_client.cpp)

if (NOT WIN32)
set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -undefined dynamic_lookup")
endif()

if (WIN32)
    set_target_properties(mqtt_client_pd PROPERTIES SUFFIX ".dll")
    set_target_properties(mqtt_client_pd PROPERTIES PREFIX "")
    set_target_properties(mqtt_client_pd PROPERTIES OUTPUT_NAME "mqtt_client")
else()
    set_target_properties(mqtt_client_pd PROPERTIES SUFFIX ".pd_darwin")
    set_target_properties(mqtt_client_pd PROPERTIES PREFIX "")
    set_target_properties(mqtt_client_pd PROPERTIES OUTPUT_NAME "mqtt_client")
endif()

target_compile_definitions(mqtt_client_pd PRIVATE BUILD_PD_OBJECT=1)

target_include_directories(mqtt_client_pd PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/pd_headers/")

target_include_directories(mqtt_client_pd PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../mqtt-object-common/src")
target_include_directories(mqtt_client_pd PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/paho.mqtt.cpp/src")
target_include_directories(mqtt_client_pd PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/paho.mqtt.c/src")

set_target_properties(mqtt_client_pd
        PROPERTIES

#        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/pd"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/pd"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/pd"
)

if (WIN32)
    target_link_libraries(mqtt_client_pd ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/pd_lib/pd.lib)
endif()

add_custom_command(TARGET mqtt_client_pd 
PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy
${CMAKE_CURRENT_SOURCE_DIR}/pd-help/mqtt_client-help.pd ${CMAKE_SOURCE_DIR}/bin/pd/mqtt_client-help.pd
)

target_link_libraries(mqtt_client_pd mqtt-object)

# max object

add_library(mqtt_client_max MODULE)
SET_TARGET_PROPERTIES(mqtt_client_max PROPERTIES BUNDLE TRUE)
target_sources(mqtt_client_max PRIVATE src/mqtt_client.cpp)

if (NOT WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -undefined dynamic_lookup")
endif()

if (WIN32)
    target_compile_definitions(mqtt_client_max PRIVATE WIN_VERSION)
endif()

if (NOT WIN32)
set_target_properties(mqtt_client_max PROPERTIES SUFFIX ".mxo")
set_target_properties(mqtt_client_max PROPERTIES BUNDLE_EXTENSION "mxo")
set_target_properties(mqtt_client_max PROPERTIES PREFIX "")
set_target_properties(mqtt_client_max PROPERTIES OUTPUT_NAME "mqtt_client")
else()
    set_target_properties(mqtt_client_max PROPERTIES SUFFIX ".mxe")
    set_target_properties(mqtt_client_max PROPERTIES BUNDLE_EXTENSION "mxe")
    set_target_properties(mqtt_client_max PROPERTIES PREFIX "")
    set_target_properties(mqtt_client_max PROPERTIES OUTPUT_NAME "mqtt_client")
endif()

if (WIN32)
    target_link_libraries(mqtt_client_max
        "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/max-sdk/source/c74support/max-includes/x64/MaxAPI.lib"
#        "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/max-sdk/source/c74support/max-includes/x64/maxcrt.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/max-sdk/source/c74support/max-includes/x64/maxcrt_p.lib")
endif()

target_compile_definitions(mqtt_client_max PRIVATE BUILD_MAX_OBJECT=1)

#target_include_directories(mqtt_client_max PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/pd_headers/")

target_include_directories(mqtt_client_max PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/max-sdk/source/c74support/max-includes")

target_include_directories(mqtt_client_max PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../mqtt-object-common/src")
target_include_directories(mqtt_client_max PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/paho.mqtt.cpp/src")
target_include_directories(mqtt_client_max PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/paho.mqtt.c/src")

set_target_properties(mqtt_client_max
        PROPERTIES

#        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/package/"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/max"
)

add_custom_command(TARGET mqtt_client_max
PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy
${CMAKE_CURRENT_SOURCE_DIR}/max-help/mqtt_client-help.maxpat ${CMAKE_SOURCE_DIR}/bin/max/mqtt_client-help.maxpat
)

target_link_libraries(mqtt_client_max mqtt-object)
