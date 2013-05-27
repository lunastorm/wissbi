cmake_minimum_required(VERSION 2.8)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    include_directories(${PROJECT_SOURCE_DIR}/3rd_party/libcxx/include .)
    link_directories(${PROJECT_BINARY_DIR}/3rd_party/libcxxrt/lib)
    add_definitions("-std=c++11")
    set(LINK_LIBS cxx dl)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    include_directories(.)
    add_definitions("-std=c++0x")
    set(LINK_LIBS pthread)
endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

add_executable(wissbi-pub pub.cpp)
add_executable(wissbi-sub sub.cpp)
add_executable(wissbi-count count.cpp)
add_executable(wissbi-record record.cpp)

target_link_libraries(
    wissbi-pub ${LINK_LIBS}
)

target_link_libraries(
    wissbi-sub ${LINK_LIBS}
)

target_link_libraries(
    wissbi-count ${LINK_LIBS}
)

target_link_libraries(
    wissbi-record ${LINK_LIBS}
)
