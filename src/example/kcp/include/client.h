#pragma once
#include <memory>
#include <iostream>
#include <string>
#include <atomic>

#include "util.h"
#ifndef USE_TCP
#include "udpsocket.h"
#include "ikcp.h"
#else
#include "socket.h"
#endif

extern std::atomic<uint32_t> total_kcp_snd_pk;
extern std::atomic<uint64_t> total_snd_data;

extern std::atomic<uint32_t> total_kcp_rcv_pk;
extern std::atomic<uint64_t> total_rcv_data;

extern std::atomic<uint32_t> finished_cnt;

class Client
{
public:
    Client( const char * ip, uint16_t port, uint32_t conv );
    ~Client();

    void one_loop();
    void loop();
    void input();

    void setmode( int mode );
    void setauto( bool _auto, int test_times = 100, int max_len = 2000 )
    {
        auto_test = _auto;
        test_count = test_times;
        str_max_len = max_len;
    }
    void setlostrate( int lostrate )
    {
        lost_rate = lostrate;
        socket_->setLostrate( lostrate / 2 );
    }
    void setsendinterval( int sendinterval )
    {
        send_interval = sendinterval;
    }
    void set_show_info( bool show_ )
    {
        show_info = show_;
    }
    void set_show_detail( bool show_ )
    {
        show_detail = show_;
    }
    void terminate()
    {
        is_running = false;
    }
    void set_delay( bool delay )
    {
        add_delay = delay;
    }

    int getFd() const { return socket_ ? socket_->getFd() : -1; }

private:
    void send( const char * data, size_t len );
    uint32_t recv( const char * data, size_t len );
    void auto_send();

private:
#ifndef USE_TCP
    std::unique_ptr<UdpSocket> socket_;
    ikcpcb * kcp;
#else
    std::unique_ptr<Socket> socket_;
#endif
    int md;
    int str_max_len;
    bool auto_test = false;
    uint32_t test_count = 10;
    int lost_rate = 0;
    int send_interval = 20;
    uint32_t sn = 0;
    uint32_t next = 0;
    uint32_t sumrtt = 0;
    uint32_t count = 0;
    uint32_t maxrtt = 0;
    bool show_info = false;
    bool show_detail = false;
    bool is_running = true;
    std::string m_readBuffer;
    uint32_t m_recvBytes = 0;
    bool add_delay = false;
    bool is_first = true;
};
