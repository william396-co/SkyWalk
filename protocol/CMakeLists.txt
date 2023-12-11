
include_directories( ${PROTOBUF_INCLUDE_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

# protobuf
file(GLOB PROTO_DEFS *.proto)

# generate
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS DESCRIPTORS PROTO_DESCS ${PROTO_DEFS} )

file(GLOB FACTORY_SRCS ${MESSAGE_FACTORY_PATH}/*.h ${MESSAGE_FACTORY_PATH}/*.cpp)

source_group("" FILES ${FACTORY_SRCS})
source_group(protocol FILES ${PROTO_HDRS} ${PROTO_SRCS})

add_library(protocol ${PROTO_SRCS} ${PROTO_HDRS} ${FACTORY_SRCS})

set_property(TARGET protocol PROPERTY FOLDER "thirdparty")
set(GENERATE_PROTOCOL_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "Generate Protocol Dir" )
set(GENERATE_PROTOCOL_DESCS "${PROTO_DESCS}" CACHE INTERNAL "Generate Protocol Descriptors" )

add_custom_command( 
	TARGET protocol POST_BUILD
	COMMAND rm -rf ${LUA_PROTOBUF_PATH}/*.desc
	COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/*.desc ${LUA_PROTOBUF_PATH}/  
)

