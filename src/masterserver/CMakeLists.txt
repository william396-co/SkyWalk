
set(BUILD_VERSION "1.0.0")
set(BUNDLE_NAME "MasterServer")

find_package(CURL REQUIRED)
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

file(GLOB DIR_SRCS *.h *.cpp ${CMAKE_CURRENT_BINARY_DIR}/version.h)

configure_file(
	"${PROJECT_SOURCE_DIR}/cmake/version.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/version.h" )

set(FILE_SRCS
	${PROJECT_SOURCE_DIR}/src/extra/json.cpp
	${PROJECT_SOURCE_DIR}/src/extra/httpsvc.cpp
	${PROJECT_SOURCE_DIR}/src/extra/ticksvc.cpp
	${PROJECT_SOURCE_DIR}/src/extra/masterproc.cpp)

source_group( "" FILES ${DIR_SRCS} )
source_group( extra FILES ${FILE_SRCS} )

add_executable(masterd ${DIR_SRCS} ${FILE_SRCS})

target_link_libraries(masterd http curlxx io message utils domain)
target_link_libraries(masterd ${EVLITE_LIBRARY} ${CURL_LIBRARIES} pthread m)

if ( (CMAKE_BUILD_TYPE MATCHES "Release") OR (CMAKE_BUILD_TYPE MATCHES "Beta") )
	target_link_libraries(masterd jemalloc)
endif()

set_property(TARGET masterd PROPERTY FOLDER "gameapps")
