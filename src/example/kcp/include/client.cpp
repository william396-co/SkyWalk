#include "client.h"

#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <string>
#include <cstdlib>
#include "random_util.h"

void rand_str( std::string & str, size_t max = 2000 )
{
    str = random_string( max - 500, max );
}

Client::Client( const char * ip, uint16_t port, uint32_t conv )
    : socket_ { nullptr }, md { 0 }
{
#ifndef USE_TCP
    socket_ = std::make_unique<UdpSocket>();
#else
    socket_ = std::make_unique<Socket>();
    socket_->setNonblocking();
#endif
    if ( !socket_->connect( ip, port ) ) {
        throw std::runtime_error( "socket_ connect failed" );
    }
#ifndef USE_TCP
    kcp = ikcp_create( conv, socket_.get() );
    ikcp_setoutput( kcp, util::kcp_output );
#endif
}

Client::~Client()
{
#ifndef USE_TCP
    ikcp_release( kcp );
#else
    socket_->close();
#endif
}

void Client::setmode( int mode )
{
#ifndef USE_TCP
    util::ikcp_set_mode( kcp, mode );
    md = mode;
#endif
}

void Client::send( const char * data, size_t len )
{
    char buff[BUFFER_SIZE] = {};
    ( (uint32_t *)buff )[0] = sn++;
    ( (uint32_t *)buff )[1] = util::iclock();
    ( (uint32_t *)buff )[2] = (uint32_t)len;

    if ( show_info ) {
        if ( show_detail )
            printf( "Send fd:%d sn:%u size:%lu content {%s}\n",  getFd(), sn - 1, len + 12, data );
        else
            printf( "Send fd:%d sn:%u size:%lu\n", getFd(), sn - 1, len + 12 );
    }
    memcpy( &buff[12], data, len );
#ifndef USE_TCP
    ikcp_send( kcp, buff, len + 12 );
    ikcp_update( kcp, util::iclock() );
#else
    socket_->send( buff, len + 12 );
#endif
    total_kcp_snd_pk.fetch_add( 1, std::memory_order_relaxed );
    total_snd_data.fetch_add( len + 12, std::memory_order_relaxed );
}

void Client::input()
{
    std::string writeBuffer;
    while ( is_running ) {
        printf( "Please enter a string to send to server(%s:%d):\n", socket_->getRemoteIp(), socket_->getRemotePort() );

        writeBuffer.clear();
        std::getline( std::cin, writeBuffer );
        if ( !writeBuffer.empty() ) {
            send( writeBuffer.data(), writeBuffer.size() );
        }
    }
}

uint32_t Client::recv( const char * data, size_t len )
{
    if ( data && len ) {
        m_readBuffer.append( data, len );
        m_recvBytes += len;

        total_rcv_data.fetch_add( len, std::memory_order_relaxed );
    }

    uint32_t ret = 0;
    char * ptr_ = m_readBuffer.data();
    while ( m_recvBytes > 0 ) {

        uint32_t sn_ = *(uint32_t *)( ptr_ );
        uint32_t ts_ = *(uint32_t *)( ptr_ + 4 );
        uint32_t sz_ = *(uint32_t *)( ptr_ + 8 ) + 12;

        if ( sz_ > m_recvBytes ) {
            break;
        }

        uint32_t rtt_ = util::iclock() - ts_;
        if ( add_delay ) {
            rtt_ += random( 60, 125 );
        }
        if ( sn_ != (uint32_t)next ) {
            printf( "fd:%d ERROR sn %u<-> next=%d\n", getFd(), sn, next );
            is_running = false;
            finished_cnt.fetch_add( 1, std::memory_order_relaxed );
        }
        ++next;
        sumrtt += rtt_;
        ++count;
        maxrtt = rtt_ > maxrtt ? rtt_ : maxrtt;
        total_kcp_rcv_pk.fetch_add( 1, std::memory_order_relaxed );

        if ( show_info ) {
            if ( show_detail )
                printf( "[RECV] fd:%u mode=%d sn:%d rrt:%d size:%u  content: {%s}\n", socket_->getFd(), md, sn_, rtt_, sz_, (char *)&ptr_[12] );
            else
                printf( "[RECV] fd:%u port:%d mode=%d sn:%d rrt:%d size:%u \n", socket_->getFd(), socket_->getLocalPort(), md, sn_, rtt_, sz_ );
        }

        if ( next >= test_count ) {
            if ( show_info ) {
                printf( "fd: %d Finished %d times test\n", getFd(), test_count );
            }
            is_running = false;
            finished_cnt.fetch_add( 1, std::memory_order_relaxed );
        }
        ptr_ += sz_;
        m_recvBytes -= sz_;
        ret = sn_;
    }

    if ( m_recvBytes == 0 ) {
        m_readBuffer.clear();
    } else {
        m_readBuffer.substr( m_readBuffer.size() - m_recvBytes );
    }
    return ret;
}

void Client::one_loop()
{
#ifndef USE_TCP
    ikcp_update( kcp, util::iclock() );
#endif
    // first send
    if ( auto_test && is_first ) {
        auto_send();
        is_first = false;
    }

    // recv pack
    uint32_t current_sn;
#ifndef USE_TCP
    if ( socket_->recv() < 0 ) {
        return;
    }
    ikcp_input( kcp, socket_->getRecvBuffer(), socket_->getRecvSize() );
    char buff[BUFFER_SIZE];
    bzero( buff, sizeof( buff ) );
    int rc = ikcp_recv( kcp, buff, sizeof( buff ) );
    if ( rc < 0 ) return;
    current_sn = recv( buff, rc );
#else
    socket_->recv();
    if ( socket_->getRecvSize() <= 0 ) {
        return;
    }
    current_sn = recv( socket_->getRecvBuffer(), socket_->getRecvSize() );
#endif
    // after recevied then send back
    if ( auto_test && !is_first && current_sn + 1 == sn && sn < test_count ) {
        auto_send();
    }
}

void Client::auto_send()
{
    std::string writeBuffer;
    rand_str( writeBuffer, str_max_len );
    if ( !writeBuffer.empty() ) {
        send( writeBuffer.data(), writeBuffer.size() );
    }
}

void Client::loop()
{
     while ( is_running ) {
        std::this_thread::sleep_for( std::chrono::milliseconds { 1 } );
        one_loop();
    }

    /* summary */
    if ( count > 0 ) {
#ifndef USE_TCP
        printf( "\nFD=[%d] MODE=[%d] DATASIZE=[%d] LOSTRATE=[{%u/%u} = %.5f] avgrtt=%d maxrtt=%d count=%d \n",
            getFd(),
            md,
            str_max_len,
            kcp->resend_cnt,
            kcp->snd_nxt,
            (double)kcp->resend_cnt / kcp->snd_nxt,
            int( sumrtt / count + 1 ),
            maxrtt,
            count + 1 );
#else
        printf( "\nFD=[%d] MODE=[TCP] DATASIZE=[%d] LOSTRATE=[{%u/%u} = %.5f] avgrtt=%d maxrtt=%d count=%d \n",
            getFd(),
            str_max_len,
            0,
            0,
            0.0f,
            int( sumrtt / count ),
            maxrtt,
            count );
#endif
    }
}
