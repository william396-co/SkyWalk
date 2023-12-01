file(GLOB DIR_SRCS *.hpp *.h *cpp *.h *.c)

source_group("" FILES ${DIR_SRCS})

add_library(utils STATIC ${DIR_SRCS})
set_property(TARGET utils PROPERTY FOLDER "share")
