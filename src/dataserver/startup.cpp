
#include "startup.h"

#include "config.h"
#include "base/base.h"
#include "utils/file.h"

using namespace data;

//
// 注册DATAD的方法
//
static PyMethodDef DatadMethods[] = {
    // 日志
    { "logger", StartupScripts::DATAD_logger, METH_VARARGS, "Get Datad Logger" },
    // 获取数据库主机地址和端口号
    { "myendpoint", StartupScripts::DATAD_myendpoint, METH_NOARGS, "Get DB Endpoint" },
    // 获取数据库访问权限
    { "mytoken", StartupScripts::DATAD_mytoken, METH_NOARGS, "Get DB Token" },
    { "mydatabase", StartupScripts::DATAD_mydatabase, METH_NOARGS, "Get DB Database" },
    { "mycharsets", StartupScripts::DATAD_mycharsets, METH_NOARGS, "Get DB CharSets" },
    { "scripts", StartupScripts::DATAD_scripts, METH_NOARGS, "Get Scripts" },
    // TODO: 继续添加
    { nullptr, nullptr, 0, nullptr }
};

#if PY_MAJOR_VERSION >= 3
#define PyString_AsString PyUnicode_AsUTF8
#define PyString_FromString PyUnicode_FromString

static struct PyModuleDef datadmodule = {
    PyModuleDef_HEAD_INIT, "datad", nullptr, -1, DatadMethods, nullptr, nullptr, nullptr, nullptr
};

PyMODINIT_FUNC PyInit_datad( void )
{
    return PyModule_Create( &datadmodule );
}
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

StartupScripts::StartupScripts()
{
#if PY_MAJOR_VERSION >= 3
    PyImport_AppendInittab( "datad", PyInit_datad );
#endif

    // 初始化Python的环境
    Py_InitializeEx( 0 );

    // 注册datad的方法
#if PY_MAJOR_VERSION < 3
    // 2.X
    Py_InitModule( "datad", DatadMethods );
#endif
}

StartupScripts::~StartupScripts()
{
    // 销毁Python的环境
    Py_Finalize();
}

void StartupScripts::addpath( const std::string & path )
{
    // 读取环境变量中的eggs
    std::vector<std::string> eggs;
    utils::PathUtils( path ).traverse( eggs, "egg" );
    utils::PathUtils( path ).traverse( eggs, "whl" );

    // 从Python获取sys.path信息
    PyObject * sys = PyImport_ImportModule( "sys" );
    if ( sys == nullptr ) {
        return;
    }
    PyObject * pypath = PyObject_GetAttrString( sys, "path" );
    if ( pypath == nullptr ) {
        return;
    }

    // 添加eggs
    for ( size_t i = 0; i < eggs.size(); ++i ) {
        PyList_Append( pypath, PyString_FromString( eggs[i].c_str() ) );
    }

    // 环境变量
    PyList_Append( pypath, PyString_FromString( path.c_str() ) );
    for ( int32_t i = 0; i < PyList_Size( pypath ); ++i ) {
        PyString_AsString( PyList_GetItem( pypath, i ) );
    }

    // 销毁sys
    Py_DECREF( sys );
}

bool StartupScripts::run( const char * scripts )
{
    // 打开启动脚本
    std::FILE * fp = std::fopen( scripts, "r" );
    if ( fp == nullptr ) {
        return false;
    }

    // 调用启动脚本
    PyRun_SimpleFile( fp, scripts );
    std::fclose( fp );

    return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 日志句柄
PyObject * StartupScripts::DATAD_logger( PyObject * self, PyObject * args )
{
    int32_t level = 0;
    char * content = nullptr;

    if ( !PyArg_ParseTuple( args, "is", &level, &content ) ) {
        return nullptr;
    }

    LOGGER( level, content );
    Py_RETURN_NONE;
}

// 数据库主机地址和端口号
PyObject * StartupScripts::DATAD_myendpoint( PyObject * self, PyObject * args )
{
    return Py_BuildValue( "sh",
        g_AppConfig.getDBHost().c_str(),
        g_AppConfig.getDBPort() );
}

// 数据库访问权限
PyObject * StartupScripts::DATAD_mytoken( PyObject * self, PyObject * args )
{
    return Py_BuildValue( "ss",
        g_AppConfig.getDBUsername().c_str(),
        g_AppConfig.getDBPassword().c_str() );
}

// 数据库
PyObject * StartupScripts::DATAD_mydatabase( PyObject * self, PyObject * args )
{
    return Py_BuildValue( "s", g_AppConfig.getDatabase().c_str() );
}

// 数据库字符集
PyObject * StartupScripts::DATAD_mycharsets( PyObject * self, PyObject * args )
{
    return Py_BuildValue( "s", g_AppConfig.getDBCharsets().c_str() );
}

// 脚本
PyObject * StartupScripts::DATAD_scripts( PyObject * self, PyObject * args )
{
    return Py_BuildValue( "ss", DATASERVER_BUILD_SCRIPT, DATASERVER_MODIFY_SCRIPT );
}
