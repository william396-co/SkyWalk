file(GLOB DIR_SRCS *.hpp *.h *cpp *.h *.c)

source_group("" FILES ${DIR_SRCS})

add_library(redis STATIC ${DIR_SRCS})
set_property(TARGET redis PROPERTY FOLDER "thirdparty")
