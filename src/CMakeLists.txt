add_executable(wissbi-pub pub.cpp)
add_executable(wissbi-sub sub.cpp)
add_executable(wissbi-count count.cpp)
add_executable(wissbi-record record.cpp)

target_link_libraries(
    wissbi-pub ${WISSBI_LINK_LIBS}
)

target_link_libraries(
    wissbi-sub ${WISSBI_LINK_LIBS}
)

target_link_libraries(
    wissbi-count ${WISSBI_LINK_LIBS}
)

target_link_libraries(
    wissbi-record ${WISSBI_LINK_LIBS}
)
