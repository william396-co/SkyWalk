file(GLOB DIR_SRCS *.h *.cpp *.hpp *.cc) 

source_group( "" FILES ${DIR_SRCS} )
add_executable(skiplist ${DIR_SRCS})
target_link_libraries(skiplist utils domain) 
if(NOT WIN32)
		target_link_libraries(skiplist pthread)
endif()

set_property(TARGET skiplist PROPERTY FOLDER "examples")
