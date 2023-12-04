set(SKIPLIST_TEST_SRC main.cpp)

add_executable(skiplist_example ${SKIPLIST_TEST_SRC})
target_link_libraries(skiplist_example utils)
