
# 生成配置文件代码
add_custom_target(cfggenerate
	COMMAND rm -rf ${PROJECT_SOURCE_DIR}/src/share/configure/meta/*.hpp 
	COMMAND mkdir -p ${PROJECT_SOURCE_DIR}/src/share/configure/meta
	COMMAND bash ${PROJECT_SOURCE_DIR}/tools/cfgexport/helper.sh ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src/share/configure/meta ${PROJECT_PROFILE}
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tools/cfgexport
)

# 生成数据库代码
add_custom_target(sqlbind
	COMMAND bash ${PROJECT_SOURCE_DIR}/tools/sqlbind/helper.sh dump ${PROJECT_SOURCE_DIR} ${PROJECT_PROFILE} 
	COMMAND rm -rf ${PROJECT_SOURCE_DIR}/src/thirdparty/scheme/*.?pp
	COMMAND rm -rf ${PROJECT_SOURCE_DIR}/tools/sqlbind/scheme
	COMMAND bash ${PROJECT_SOURCE_DIR}/tools/sqlbind/helper.sh bind ${PROJECT_SOURCE_DIR} ${PROJECT_PROFILE} 
	COMMAND bash ${PROJECT_SOURCE_DIR}/tools/sqlbind/helper.sh bindgo ${PROJECT_SOURCE_DIR} ${PROJECT_PROFILE} 
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tools/sqlbind
)



