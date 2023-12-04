file(GLOB DIR_SRCS *.h *.cpp *.hpp *.cc) 

source_group( "" FILES ${DIR_SRCS} )
add_executable(unique_example ${DIR_SRCS})
target_link_libraries(unique_example utils domain) 

set_property(TARGET unique_example PROPERTY FOLDER "examples")
