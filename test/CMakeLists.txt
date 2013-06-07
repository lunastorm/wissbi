
SET(WISSBI_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")

SET(GMOCK_ROOT "${PROJECT_SOURCE_DIR}/3rd_party/gmock-1.6.0")
SET(GMOCK_INCLUDE_DIR "${GMOCK_ROOT}/include")

SET(GTEST_ROOT "${GMOCK_ROOT}/gtest")
SET(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")

include_directories(${WISSBI_SOURCE_DIR})
include_directories(${GMOCK_INCLUDE_DIR})
include_directories(${GTEST_INCLUDE_DIR})

add_executable(runUnitTests
    unit/sub_entry_test.cpp
    unit/sub_dir_test.cpp
    unit/util_test.cpp
    unit/line_io_policy_test.cpp
    unit/msg_filter_test.cpp
    unit/length_io_policy_test.cpp
    unit/sysv_mq_io_policy_test.cpp
    unit/tcp_io_policy_test.cpp
    unit/tee_io_policy_test.cpp
    unit/string_io_policy_test.cpp
    unit/metric_reporter_test.cpp
)

target_link_libraries(runUnitTests gtest gtest_main gmock pthread)

add_custom_target (integration ALL)
add_custom_command (
    SOURCE integration
    TARGET integration
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/integration ${CMAKE_CURRENT_BINARY_DIR}/integration
)

add_custom_target (runIntegrationTests ALL)
add_custom_command (
    SOURCE runIntegrationTests
    TARGET runIntegrationTests
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/integration
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/runIntegrationTests ${CMAKE_CURRENT_BINARY_DIR}/runIntegrationTests
)

add_test(
    NAME runUnitTests
    COMMAND runUnitTests --gtest_output=xml
)

add_test(
    NAME runIntegrationTests
    COMMAND runIntegrationTests
)
