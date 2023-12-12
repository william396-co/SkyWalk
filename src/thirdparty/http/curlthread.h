
#ifndef __SRC_HTTP_CURLTHREAD_H__
#define __SRC_HTTP_CURLTHREAD_H__

#include "xcurl.h"
#include "utils/thread.h"

// CURL线程
class CurlThread final : public utils::IThread
{
public:
    CurlThread();
    virtual ~CurlThread() final;
    virtual bool onStart() final;
    virtual void onExecute() final;
    virtual void onStop() final;

public:
    // 发送请求
    bool send( HttpMethod method,
        CurlRequest * request, int32_t timeout );

private:
    CurlAgent * m_CurlAgent;
};

// CURL线程组
class CurlThreads
{
public:
    CurlThreads( uint8_t nthreads );
    ~CurlThreads();

public:
    // 初始化线程组
    bool initialize();
    // 销毁线程组
    void finalize();
    // 轮换线程
    CurlThread * rotate() const;

private:
    uint8_t m_Number;
    std::vector<CurlThread *> m_Threads;
};

#endif
