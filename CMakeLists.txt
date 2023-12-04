cmake_minimum_required(VERSION 3.9)

project(SkyWalk)

set(CMAKE_CXX_STANDARD 20)

set(CXX_FLAGS
		-ggdb
		-Og
		-O0
		-Wall
		-Wextra
		-Werror
)

# set output path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# include folder
include_directories(include)
include_directories(src)
include_directories(src/share)

# protocal
#add_subdirectory(protocal)

# public lib
add_subdirectory(src/share/utils)
add_subdirectory(src/share/domain)

# thirdparty lib
#add_subdirectory(src/thirdparty/io)
#add_subdirectory(src/thirdparty/redis)
#add_subdirectory(src/thirdparty/mysql)
#add_subdirectory(src/thirdparty/lua)
#add_subdirectory(src/thirdparty/http)
#add_subdirectory(src/thirdparty/rapidjson)
#add_subdirectory(src/thirdparty/protocal)

# sub project
if(NOT WIN32)
	add_subdirectory(src/guard)
endif()
#add_subdirectory(src/gateserver)
#add_subdirectory(src/dataserver)
#add_subdirectory(src/masterserver)
#add_subdirectory(src/worldserver)
#add_subdirectory(src/sceneserver)

# test project

#if(BUILD_EXAMPLS)
	add_subdirectory(src/example/uniqueid)
	add_subdirectory(src/example/behavior_tree)
	add_subdirectory(src/example/skiplist)
#	add_subdirectory(src/example/redis)
	#	add_subdirectory(src/example/io)
	#	add_subdirectory(src/example/lua)
	#	add_subdirectory(src/example/kcp/kcp_client)
	#	add_subdirectory(src/example/kcp/kcp_server)
	#	add_subdirectory(src/example/kcp/kcp_multi_client)
	#endif()
