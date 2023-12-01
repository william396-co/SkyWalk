include_directories(${PROJECT_SOURCE_DIR}/src/examples/kcp_test/include)

file(GLOB DIR_SRCS *.h *.cpp *.hpp *.cc *.c) 
file(GLOB INCLUDE_SRCS ${PROJECT_SOURCE_DIR}/src/examples/kcp_test/include/*.h 
	   	${PROJECT_SOURCE_DIR}/src/examples/kcp_test/include/*.cpp 
		${PROJECT_SOURCE_DIR}/src/examples/kcp_test/include/*.c)

source_group( "" FILES ${DIR_SRCS} )
source_group( include FILES ${INCLUDE_SRCS})

add_executable(kcp_client ${DIR_SRCS} ${INCLUDE_SRCS})
target_link_libraries(kcp_client pthread) 

set_property(TARGET kcp_client PROPERTY FOLDER "examples")
