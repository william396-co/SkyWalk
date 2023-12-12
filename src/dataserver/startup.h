
#ifndef __SRC_DATASERVER_STARTUP_H__
#define __SRC_DATASERVER_STARTUP_H__

#include <Python.h>
#include <vector>
#include <string>
#include <cstdio>
#include <cstring>

#include "base/base.h"

//
// 启动脚本
//
// NOTICE: 脚本加载时DataServer未启动
// 流程:
//      1. 解析命令行
//      2. 日志系统初始化
//      3. 系统配置初始化
//      4. ****** 初始化启动脚本 ******
//      5. 配置中心初始化
//      6. DataServer启动
//      7. DataServer运行中...
//      8. DataServer销毁
//

class StartupScripts
{
public:
    StartupScripts();
    ~StartupScripts();

public:
    // 运行脚本
    bool run( const char * scripts );
    // 注册环境变量
    void addpath( const std::string & path );

public:
    // 日志
    static PyObject * DATAD_logger( PyObject * self, PyObject * args );
    // 获取数据库主机地址和端口号
    static PyObject * DATAD_myendpoint( PyObject * self, PyObject * args );
    // 获取数据库访问权限
    static PyObject * DATAD_mytoken( PyObject * self, PyObject * args );
    // 获取数据库
    static PyObject * DATAD_mydatabase( PyObject * self, PyObject * args );
    // 获取数据库字符集
    static PyObject * DATAD_mycharsets( PyObject * self, PyObject * args );
    // 脚本定义
    static PyObject * DATAD_scripts( PyObject * self, PyObject * args );
};

#endif
