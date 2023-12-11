
include_directories( ${GENERATE_PROTOCOL_DIR} )

FILE(GLOB DIR_SRCS *.h *.cpp)
ADD_LIBRARY(extra ${DIR_SRCS})

source_group("" FILES ${DIR_SRCS})
