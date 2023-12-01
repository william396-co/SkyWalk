file(GLOB DIR_SRCS *.hpp *.h *cpp *.h *.c)

source_group("" FILES ${DIR_SRCS})

add_library(domain STATIC ${DIR_SRCS})
set_property(TARGET domain PROPERTY FOLDER "share")
