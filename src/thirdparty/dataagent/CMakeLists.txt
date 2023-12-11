
file(GLOB DIR_SRCS *.h *.cpp *.hpp)
file(GLOB IMPL impl/*.h impl/*.cpp)

source_group("" FILES ${DIR_SRCS})
source_group(impl FILES ${IMPL})

add_library(dataagent ${DIR_SRCS} ${IMPL})
set_property(TARGET dataagent PROPERTY FOLDER "thirdparty")
