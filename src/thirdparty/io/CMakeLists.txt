
FILE(GLOB_RECURSE DIR_SRCS *.h *.cpp)
ADD_LIBRARY(io ${DIR_SRCS})

source_group("" FILES ${DIR_SRCS})
set_property(TARGET io PROPERTY FOLDER "thirdparty")