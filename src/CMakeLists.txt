cmake_minimum_required(VERSION 2.6)

SET(CMAKE_BUILD_TYPE Debug)
ADD_DEFINITIONS("-std=c++0x")

SET(CMAKE_INSTALL_RPATH "/opt/wissbi/lib")
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

IF(APPLE)
    SET(UDT_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/../tmp/udt4/src/libudt.dylib")
ELSE()
    SET(UDT_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/../tmp/udt4/src/libudt.so")
ENDIF()

include_directories(
    ../tmp/udt4/src
)

add_executable(pub pub.cpp)
add_executable(sub sub.cpp)

target_link_libraries(
    pub ${UDT_LIBRARY} pthread
)

target_link_libraries(
    sub ${UDT_LIBRARY} pthread
)
