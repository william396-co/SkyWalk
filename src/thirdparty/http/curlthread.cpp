
#include "utils/random.h"
#include "curlthread.h"

CurlThread::CurlThread()
{}

CurlThread::~CurlThread()
{}

bool CurlThread::onStart()
{
    m_CurlAgent = new CurlAgent;
    if ( m_CurlAgent == nullptr ) {
        return false;
    }

    if ( !m_CurlAgent->initialize() ) {
        return false;
    }

    return true;
}

void CurlThread::onExecute()
{
    m_CurlAgent->dispatch();
}

void CurlThread::onStop()
{
    if ( m_CurlAgent != nullptr ) {
        m_CurlAgent->finalize();
        delete m_CurlAgent;
        m_CurlAgent = nullptr;
    }
}

bool CurlThread::send( HttpMethod method, CurlRequest * request, int32_t timeout )
{
    return m_CurlAgent->perform( method, request, timeout );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CurlThreads::CurlThreads( uint8_t nthreads )
    : m_Number( nthreads )
{
    ::curl_global_init( CURL_GLOBAL_DEFAULT );
}

CurlThreads::~CurlThreads()
{
    ::curl_global_cleanup();
}

bool CurlThreads::initialize()
{
    for ( uint8_t i = 0; i < m_Number; ++i ) {
        CurlThread * thread = new CurlThread;
        if ( thread == nullptr ) {
            break;
        }
        // 设置线程属性
        thread->setDetach();
        // 线程开始运行
        if ( !thread->start() ) {
            break;
        }
        // 放入管理器中
        m_Threads.push_back( thread );
    }

    if ( m_Threads.size() != m_Number ) {
        finalize();
        return false;
    }

    return true;
}

void CurlThreads::finalize()
{
    for ( const auto & thread : m_Threads ) {
        if ( thread != nullptr ) {
            thread->stop();
            delete thread;
        }
    }

    m_Number = 0;
    m_Threads.clear();
}

CurlThread * CurlThreads::rotate() const
{
    return *utils::random_thread_local::get( m_Threads );
}
