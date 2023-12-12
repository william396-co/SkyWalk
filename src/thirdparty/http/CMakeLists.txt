
FILE(GLOB DIR_SRCS *.h *.cpp)

SET(HTTP_SRCS
	${PROJECT_SOURCE_DIR}/src/thirdparty/http/url.h ${PROJECT_SOURCE_DIR}/src/thirdparty/http/url.cpp
	${PROJECT_SOURCE_DIR}/src/thirdparty/http/base64.h ${PROJECT_SOURCE_DIR}/src/thirdparty/http/base64.cpp
	${PROJECT_SOURCE_DIR}/src/thirdparty/http/parser.h ${PROJECT_SOURCE_DIR}/src/thirdparty/http/parser.cpp
	${PROJECT_SOURCE_DIR}/src/thirdparty/http/message.h ${PROJECT_SOURCE_DIR}/src/thirdparty/http/message.cpp)

ADD_LIBRARY(http ${HTTP_SRCS})
ADD_LIBRARY(curlxx ${DIR_SRCS})

source_group("" FILES ${DIR_SRCS})

set_property(TARGET http PROPERTY FOLDER "thirdparty")
set_property(TARGET curlxx PROPERTY FOLDER "thirdparty")
