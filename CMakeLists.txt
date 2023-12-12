cmake_minimum_required(VERSION 3.9)

project(SkyWalk C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_FLAGS "-std=c11")

# Enviorment setting
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()
if(NOT BUILD_PLATFORM)
	set(BUILD_PLATFORM "DEV")
endif()
if(NOT PROJECT_OUTPUT_PATH)
	set(PROJECT_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
endif()
#set(PROJECT_PROFILE ".projectprofile")
set(CMAKE_VERBOSE_MAKEFILE OFF)
option(BUILD_EXAMPLES "Build Examples" OFF)
option(CMAKE_BUILD_MEMCHK "CMake build memchk" ON)

#include other folder
include(cmake/Misc.cmake)
include(cmake/Targets.cmake)

# CXX FLAGS
set(CXX_FLAGS
	 -ggdb
	 # -DVALGRIND
	 -DCHECK_PTHREAD_RETURN_VALUE
	 -D_FILE_OFFSET_BITS=64
	 -Wall
	 -Wextra
	 -Werror
	 #-Wconversion
	 -Wno-unused-value
	 -Wno-unused-result
	 -Wno-unused-variable
	 -Wno-unused-function
	 -Wno-unused-parameter
	 -Wno-unused-but-set-variable
	 -Wno-unused-local-typedefs
	 -Wno-class-memaccess
	 -Wno-implicit-fallthrough
	 -Wno-int-in-bool-context
	 -Wno-deprecated-copy
	 -Wno-deprecated-declarations
	 -Wno-type-limits
	 -Wno-old-style-cast
	 -Woverloaded-virtual
	 -Wpointer-arith
	 # -fstack-protector-all stack protect no need
	 -rdynamic
)
if(CMAKE_BUILD_BITS EQUAL 32)
  	list(APPEND CXX_FLAGS "-m32")
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  	list(APPEND CXX_FLAGS "-Wno-null-dereference")
  	list(APPEND CXX_FLAGS "-Wno-sign-conversion")
  	list(APPEND CXX_FLAGS "-Wno-expansion-to-defined")
  	list(APPEND CXX_FLAGS "-Wno-implicit-function-declaration")
  	list(APPEND CXX_FLAGS "-Wthread-safety")
	list(REMOVE_ITEM CXX_FLAGS "-rdynamic" "-Wno-unused-but-set-variable" "-Wno-class-memaccess")
endif()
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

if(CMAKE_BUILD_MEMCHK)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -D__DEBUG__ -fsanitize=address -fno-omit-frame-pointer")
else()
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -D__DEBUG__")
endif()
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -finline-limit=1000 -DKAGUYA_USE_STACK_TRACE=0")
set(CMAKE_CXX_FLAGS_BETA "-O3 -DNDEBUG -D__BETA__")
add_definitions( "-D__PLATFORM__=${BUILD_PLATFORM}" )


# folder definition
# RPATH
set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH true)
set(CMAKE_INSTALL_RPATH lib)
set(LUA_PROTOBUF_PATH ${PROJECT_SOURCE_DIR}/scripts/protocol)
set(MESSAGE_FACTORY_PATH ${PROJECT_SOURCE_DIR}/src/thirdparty/protocol)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# set output path definition
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/archive)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake; ${CMAKE_MODULE_PATH})

# find_package
find_package(Evlite REQUIRED)
find_package(Protobuf 3.0 REQUIRED)


message(STATUS "Build Types:" ${CMAKE_BUILD_TYPE})
message(STATUS "Build Platform:" ${BUILD_PLATFORM})
message(STATUS "Project Profile:" ${PROJECT_PROFILE})
message(STATUS "Project Output Path:" ${PROJECT_OUTPUT_PATH})


# include folder
include_directories(include)
include_directories(src)
include_directories(src/share)
include_directories(src/thirdparty)
include_directories(${EVLITE_INCLUDE_DIR})

# CMAKE default TARGET
#add_custom_target(automatic
#	ALL
#	#COMMAND cp -a gamerc.sh.example ${EXECUTABLE_OUTPUT_PATH}/gamerc.sh
#	COMMAND cp -a tools/build_database.sql ${EXECUTABLE_OUTPUT_PATH}/build_database.sql
#	COMMAND cp -a tools/modify_database.sql ${EXECUTABLE_OUTPUT_PATH}/modify_database.sql
#	COMMAND cp -a tools/startup.py.sample ${EXECUTABLE_OUTPUT_PATH}/startup.py
#	DEPENDS tools/build_database.sql tools/modify_database.sql tools/startup.py.sample
#	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#)


# link
link_directories(${PROJECT_SOURCE_DIR}/lib)

# protocal
add_subdirectory(protocol)

# public lib
add_subdirectory(src/share/utils)
add_subdirectory(src/share/domain)
add_subdirectory(src/share/message)

# thirdparty lib
add_subdirectory(src/thirdparty/io)
#add_subdirectory(src/thirdparty/redis)
add_subdirectory(src/thirdparty/mysql)
add_subdirectory(src/thirdparty/lua)
add_subdirectory(src/thirdparty/http)
#add_subdirectory(src/thirdparty/rapidjson)
add_subdirectory(src/thirdparty/scheme)

# extra
add_subdirectory(src/extra)

# sub project
if(NOT WIN32)
	add_subdirectory(src/guard)
endif()
#add_subdirectory(src/gateserver)
add_subdirectory(src/dataserver)
add_subdirectory(src/masterserver)
#add_subdirectory(src/worldserver)
#add_subdirectory(src/sceneserver)

# test project

if(BUILD_EXAMPLS)
	add_subdirectory(src/example/uniqueid)
	add_subdirectory(src/example/behavior_tree)
	add_subdirectory(src/example/skiplist)
	add_subdirectory(src/example/redis)
	add_subdirectory(src/example/io)
	add_subdirectory(src/example/lua)
	add_subdirectory(src/example/kcp/kcp_client)
	add_subdirectory(src/example/kcp/kcp_server)
	add_subdirectory(src/example/kcp/kcp_multi_client)
endif()
