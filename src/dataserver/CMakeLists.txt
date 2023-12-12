
set(BUILD_VERSION "4.0.4")
set(BUNDLE_NAME "DataServer")

configure_file(
	"${PROJECT_SOURCE_DIR}/cmake/version.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/version.h" )

find_package(Lua REQUIRED)
find_package(PythonLibs 3 REQUIRED)
find_package(MySQL REQUIRED)

include_directories( ${LUA_INCLUDE_DIR} )
include_directories( ${PYTHON_INCLUDE_PATH} )
include_directories( ${MYSQL_INCLUDE_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${PROJECT_SOURCE_DIR}/src/thirdparty/scheme )

file(GLOB DIR_SRCS *.h *.cpp *.hpp ${CMAKE_CURRENT_BINARY_DIR}/version.h)
file(GLOB SCHEME_SRCS
	${PROJECT_SOURCE_DIR}/src/thirdparty/scheme/*.h
	${PROJECT_SOURCE_DIR}/src/thirdparty/scheme/*.?pp )

set(FILE_SRCS
    ${PROJECT_SOURCE_DIR}/src/extra/ticksvc.cpp
    ${PROJECT_SOURCE_DIR}/src/extra/masterproc.cpp)

source_group( "" FILES ${DIR_SRCS} )
source_group( extra FILES ${FILE_SRCS} )
source_group( scheme FILES ${SCHEME_SRCS} )

if(APPLE)
	link_directories( ${PROJECT_SOURCE_DIR}/MacOS-Prj/libs/mysql/lib )
	include_directories( ${PROJECT_SOURCE_DIR}/MacOS-Prj/libs/mysql/include )
	link_directories( /System/Library/Frameworks/Python.framework/Versions/Current/lib )
	include_directories( /System/Library/Frameworks/Python.framework/Versions/Current/include/python2.7 )
endif()

add_executable(datad ${DIR_SRCS} ${FILE_SRCS})

target_link_libraries(datad scheme utils domain io message mysqlxx luaxx)
target_link_libraries(datad ${PYTHON_LIBRARIES} ${MYSQL_LIBRARIES} pthread ${EVLITE_LIBRARY} ${LUA_LIBRARIES})

if ( (CMAKE_BUILD_TYPE MATCHES "Release") OR (CMAKE_BUILD_TYPE MATCHES "Beta") )
	target_link_libraries(datad jemalloc)
endif()

set_property(TARGET datad PROPERTY FOLDER "gameapps")
