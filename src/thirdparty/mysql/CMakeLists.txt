
FILE(GLOB_RECURSE DIR_SRCS *.h *.cpp)
ADD_LIBRARY(mysqlxx ${DIR_SRCS})

if(APPLE)
	INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/MacOS-Prj/libs/mysql/include )
endif()

source_group("" FILES ${DIR_SRCS})
set_property(TARGET mysqlxx PROPERTY FOLDER "thirdparty")
