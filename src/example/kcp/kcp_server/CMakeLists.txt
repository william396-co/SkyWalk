
file(GLOB DIR_SRCS *.h *.cpp *.hpp *.cc) 

source_group( "" FILES ${DIR_SRCS} )

add_executable(kcp_server ${DIR_SRCS})
target_link_libraries(kcp_server io pthread ${EVLITE_LIBRARY})

set_property(TARGET kcp_server PROPERTY FOLDER "examples")
