file(GLOB DIR_SRCS *.h *.cpp)

source_group("" FILE  ${DIR_SRCS})
add_executable(behaviors ${DIR_SRCS})
target_link_libraries(behaviors domain utils)

set_property(TARGET behaviors PROPERTY FOLDER "examples")
