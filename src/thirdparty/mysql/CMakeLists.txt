file(GLOB DIR_SRCS *.hpp *.h *cpp *.h *.c)

source_group("" FILES ${DIR_SRCS})

add_library(mysql STATIC ${DIR_SRCS})
set_property(TARGET mysql PROPERTY FOLDER "thirdparty")
