cmake_minimum_required(VERSION 2.6)

SET(CMAKE_BUILD_TYPE Debug)
ADD_DEFINITIONS("-std=c++0x")

SET(CMAKE_INSTALL_RPATH "/opt/wissbi/lib")
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

include_directories(
    .
)

add_executable(pub pub.cpp)
add_executable(sub sub.cpp)

target_link_libraries(
    pub pthread
)

target_link_libraries(
    sub pthread
)
