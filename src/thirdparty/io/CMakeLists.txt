file(GLOB DIR_SRCS *.hpp *.h *cpp *.h *.c)

source_group("" FILES ${DIR_SRCS})

add_library(io STATIC ${DIR_SRCS})
set_property(TARGET io PROPERTY FOLDER "thirdparty")
