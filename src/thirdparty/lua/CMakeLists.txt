
file(GLOB_RECURSE DIR_SRCS *.h *.cpp)

# C++ 工程
file(GLOB LUA_VEC3 vec3/*.cpp vec3/*.h)
file(GLOB LUA_PROFILER profiler/*.c profiler/*.cpp profiler/*.h)
file(GLOB LUA_PROTOBUF protobuf/*.c protobuf/*.cpp protobuf/*.h)
file(GLOB LUA_RAPIDJSON rapidjson/*.c rapidjson/*.cpp rapidjson/*.h)
file(GLOB LUA_FIXPOINT fixpoint/*.c fixpoint/*.cpp fixpoint/*.h fixpoint/fpm/*.hpp )
file(GLOB LUA_FIXPOINT_FPM fixpoint/fpm/*.hpp )

# LUA
file(GLOB LUA_SCRIPTS ${PROJECT_SOURCE_DIR}/*.lua )
file(GLOB LUA_SCRIPTS_PROTO ${PROJECT_SOURCE_DIR}/protocol/*.lua ${PROJECT_SOURCE_DIR}/protocol/*.desc )
file(GLOB LUA_SCRIPTS_UTILS ${PROJECT_SOURCE_DIR}/utils/*.lua )
file(GLOB LUA_SCRIPTS_SCENE ${PROJECT_SOURCE_DIR}/scene/*.lua )
file(GLOB LUA_SCRIPTS_WORLD ${PROJECT_SOURCE_DIR}/world/*.lua )

ADD_LIBRARY(luaxx ${DIR_SRCS} ${LUA_VEC3} ${LUA_PROTOBUF} ${LUA_PROFILER} ${LUA_RAPIDJSON} ${LUA_FIXPOINT} ${LUA_FIXPOINT_FPM})

if(APPLE)
	# lua
	INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/MacOS-Prj/libs/lua/include )
endif()

source_group("" FILES ${DIR_SRCS})
source_group(vec3 FILES ${LUA_VEC3})
source_group(profiler FILES ${LUA_PROFILER})
source_group(protobuf FILES ${LUA_PROTOBUF})
source_group(rapidjson FILES ${LUA_RAPIDJSON})
source_group(fixpoint FILES ${LUA_FIXPOINT})
source_group(fixpoint\\fpm FILES ${LUA_FIXPOINT_FPM} )
source_group(scripts FILES ${LUA_SCRIPTS} ${LUA_SCRIPTS_PROTO} ${LUA_SCRIPTS_UTILS} ${LUA_SCRIPTS_SCENE} ${LUA_SCRIPTS_WORLD})

set_property(TARGET luaxx PROPERTY FOLDER "thirdparty")
