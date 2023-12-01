set(GUARD_SRCS guard.cpp)

#link_directories({PORJECT_BINARY_DIR}/lib)

add_executable(guard ${GUARD_SRCS})
target_link_libraries(guard utils)

set_property(TARGET guard PROPERTY FOLDER gameapps)
