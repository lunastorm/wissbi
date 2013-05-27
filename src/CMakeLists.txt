cmake_minimum_required(VERSION 2.6)

include_directories(${PROJECT_SOURCE_DIR}/3rd_party/libcxx/include .)
link_directories(${PROJECT_BINARY_DIR}/3rd_party/libcxxrt/lib)
add_definitions("-std=c++11")

add_executable(wissbi-pub pub.cpp)
add_executable(wissbi-sub sub.cpp)
add_executable(wissbi-count count.cpp)
add_executable(wissbi-record record.cpp)

target_link_libraries(
    wissbi-pub cxx dl
)

target_link_libraries(
    wissbi-sub cxx dl
)

target_link_libraries(
    wissbi-count cxx dl
)

target_link_libraries(
    wissbi-record cxx dl
)
