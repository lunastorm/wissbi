cmake_minimum_required(VERSION 2.6)

SET(CMAKE_BUILD_TYPE Debug)
ADD_DEFINITIONS("-std=c++0x")
project(wissbi_test)

enable_testing()

SET(WISSBI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

SET(GMOCK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../tmp/gmock-1.6.0")
SET(GMOCK_INCLUDE_DIR "${GMOCK_ROOT}/include")
SET(GMOCK_LIBRARY "${GMOCK_ROOT}/libgmock.a")

SET(GTEST_ROOT "${GMOCK_ROOT}/gtest")
SET(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")
SET(GTEST_LIBRARY "${GTEST_ROOT}/libgtest.a")
SET(GTEST_MAIN_LIBRARY "${GTEST_ROOT}/libgtest_main.a")

include_directories(${WISSBI_SOURCE_DIR})
include_directories(${GMOCK_INCLUDE_DIR})
include_directories(${GTEST_INCLUDE_DIR})

add_executable(runUnitTests
    sub_entry_test.cpp
    sub_dir_test.cpp
    util_test.cpp
    line_io_policy_test.cpp
    msg_filter_test.cpp
    length_block_policy_test.cpp
    sysvmq_wrapper_test.cpp
)

target_link_libraries(runUnitTests ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} ${GMOCK_LIBRARY} pthread)

add_test(
    NAME runUnitTests
    COMMAND runUnitTests
)
