cmake_minimum_required(VERSION 2.6)

SET(CMAKE_BUILD_TYPE Debug)
ADD_DEFINITIONS("-std=c++0x")
project(wissbi_test)

enable_testing()
#set (GMOCK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../tmp/gmock-1.6.0")
#find_package(GMock REQUIRED)
set (GTEST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../tmp/gtest-1.6.0")
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../tmp/gmock-1.6.0/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../src")

add_executable(runUnitTests
    sub_entry_test.cpp
    sub_dir_test.cpp
    util_test.cpp
    line_block_policy_test.cpp
    msg_filter_test.cpp
)

target_link_libraries(runUnitTests ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} pthread ${CMAKE_CURRENT_SOURCE_DIR}/../tmp/gmock-1.6.0/libgmock.a)

add_test(
    NAME runUnitTests
    COMMAND runUnitTests
)
