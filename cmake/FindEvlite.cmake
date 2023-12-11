
#在指定目录下寻找头文件和动态库文件的位置，可以指定多个目标路径
#find_path(EVLITE_INCLUDE_DIR network.h /usr/local/include/evlite ${CMAKE_SOURCE_DIR}/ModuleMode)
#find_path(EVLITE_INCLUDE_DIR event.h /usr/local/include/evlite ${CMAKE_SOURCE_DIR}/ModuleMode)
#find_path(EVLITE_INCLUDE_DIR threads.h /usr/local/include/evlite ${CMAKE_SOURCE_DIR}/ModuleMode)
find_path(EVLITE_INCLUDE_DIR evlite HINTS /usr/include /usr/local/include ${CMAKE_SOURCE_DIR}/ModuleMode)
#从系统路径查询
find_library(EVLITE_LIBRARY NAMES evlite HINTS ${PROJECT_SOURCE_DIR}/lib /usr/lib /usr/local/lib)

if (EVLITE_INCLUDE_DIR AND EVLITE_LIBRARY)
	set(EVLITE_FOUND TRUE)
	set(EVLITE_MESSAGE "Found Evlite: ${EVLITE_LIBRARY}, IncludeDir: ${EVLITE_INCLUDE_DIR}")
	message(STATUS ${EVLITE_MESSAGE})
endif (EVLITE_INCLUDE_DIR AND EVLITE_LIBRARY)
