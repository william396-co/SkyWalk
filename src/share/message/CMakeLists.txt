file(GLOB DIR_SRCS *.h *.cc *.cpp)

add_library(message ${DIR_SRCS})
source_group("" FILES ${DIR_SRCS})

set_property(TARGET message PROPERTY FOLDER "share")
