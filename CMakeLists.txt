cmake_minimum_required(VERSION 2.8)
project(wissbi)

enable_testing()

add_definitions("-std=c++0x")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    include_directories(${PROJECT_SOURCE_DIR}/3rd_party/libcxx/include src)
    link_directories(${PROJECT_BINARY_DIR}/3rd_party/libcxxrt/lib)
    set(WISSBI_LINK_LIBS cxx dl)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    include_directories(src)
    set(WISSBI_LINK_LIBS pthread)
endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

add_subdirectory(3rd_party)
add_subdirectory(src)
add_subdirectory(test)
