file(GLOB DIR_SRCS *.hpp *.h *cpp *.h *.c)

source_group("" FILES ${DIR_SRCS})

add_library(http STATIC ${DIR_SRCS})
set_property(TARGET http PROPERTY FOLDER "thirdparty")
