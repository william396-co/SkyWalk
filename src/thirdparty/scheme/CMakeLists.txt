
file(GLOB_RECURSE DIR_SRCS *.hpp *.cpp *.cc)

ADD_LIBRARY(scheme ${DIR_SRCS})

source_group("" FILES ${DIR_SRCS})
set_property(TARGET scheme PROPERTY FOLDER "thirdparty")
