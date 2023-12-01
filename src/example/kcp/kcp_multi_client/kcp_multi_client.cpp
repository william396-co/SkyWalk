#include <signal.h>
#include <thread>
#include <chrono>
#include <vector>
#include <future>
#include <functional>
#include <algorithm>
#include <atomic>
#include <iomanip>

#include "../include/util.h"
#include "../include/client.h"
#include "../include/joining_thread.h"

constexpr auto default_ip = "127.0.0.1";
constexpr auto default_port = 9527;
constexpr auto default_max_len = 1000;
constexpr auto default_test_times = 1000;
constexpr auto default_lost_rate = 0;
constexpr auto default_send_interval = 60; // ms
constexpr auto default_thread_cnt = 1;
constexpr auto clients_per_thread = 400;

bool g_running = true;
std::atomic<uint32_t> total_kcp_snd_pk {};
std::atomic<uint32_t> total_udp_snd_pk {};
std::atomic<uint64_t> total_udp_snd_data {};
std::atomic<uint64_t> total_snd_data {};

std::atomic<uint32_t> total_kcp_rcv_pk {};
std::atomic<uint32_t> total_udp_rcv_pk {};
std::atomic<uint64_t> total_udp_rcv_data {};
std::atomic<uint64_t> total_rcv_data {};

std::atomic<uint32_t> finished_cnt{};

void signal_handler( int sig )
{
    g_running = false;
}
void handle_signal()
{
    signal( SIGPIPE, SIG_IGN );
    signal( SIGINT, signal_handler );
    signal( SIGTERM, signal_handler );
}

using ClientPtr =  std::unique_ptr<Client>;
using ClientPtrVec =  std::vector<ClientPtr>;
using Task = std::function<void()>;

void work( ClientPtrVec::iterator beg, ClientPtrVec::iterator end )
{
    while ( g_running ) {
        for ( auto it = beg; it != end; ++it ) {
            ( *it )->one_loop();
        }
        std::this_thread::sleep_for( std::chrono::milliseconds { 2 } );
    }
}

ClientPtr start_client( const char * ip, uint16_t port, int mode, int max_len, int test_times, int lost_rate, int interval = default_send_interval )
{
    ClientPtr client = std::make_unique<Client>( ip, port, conv );
    client->setmode( mode );
    client->setauto( true, test_times, max_len );
    client->setlostrate( lost_rate );
    client->setsendinterval( interval );
  //  client->set_show_info( true );
    //  client->set_show_detail( true );
    // client->set_delay( true );
    return client;
}

int main( int argc, char ** argv )
{
    handle_signal();

    int mode = 2;
    uint16_t port = default_port;
    std::string ip = default_ip;
    int max_len = default_max_len;
    int test_times = default_test_times;
    int lost_rate = default_lost_rate;
    int send_interval = default_send_interval;
    int thread_cnt = default_thread_cnt;

    if ( argc > 1 ) {
        thread_cnt = atoi( argv[1] );
    }

    if ( argc > 2 ) {
        ip = argv[2];
    }

    if ( argc > 3 ) {
        port = atoi( argv[3] );
    }

    printf( "Usage:<%s> ThreadCount:%d ip:[%s]\n",
        argv[0],
        thread_cnt,
        ip.c_str() );

    using namespace std::chrono;
    auto now = duration_cast<milliseconds>( system_clock::now().time_since_epoch() ).count();

    ClientPtrVec clients;
    for ( auto i = 0; i != thread_cnt * clients_per_thread; ++i ) {
        clients.emplace_back( start_client( ip.c_str(), port, mode, max_len, test_times, lost_rate, send_interval ) );
    }

    std::vector<joining_thread> threads;
    auto block_start = clients.begin();
    for ( auto i = 0; i != thread_cnt; ++i ) {
        auto block_end = block_start;
        std::advance(block_end,clients_per_thread);
        threads.emplace_back( work, block_start, block_end );
        block_start = block_end;
    }

    std::cout << "client cnt:" << thread_cnt * clients_per_thread << " thread cnt:" << thread_cnt << "\n";
    while ( g_running ) {
        if ( finished_cnt == (uint32_t)( thread_cnt * clients_per_thread ) ) {
            g_running = false;
            break;
        }
        std::this_thread::sleep_for( std::chrono::milliseconds { 1 } );
    }

    for ( auto & c : clients ) {
        c->terminate();
    }

    auto diff = duration_cast<milliseconds>( system_clock::now().time_since_epoch() ).count() - now;
    std::cout << " total spend time " << diff << " millisecond\n";

    // TOTAL REPORT
    diff /=1000.0f;
    using  namespace util;
    std::cout << "\nSEND\tKCP\tUDP\tKCP_DATA\tUDP_DATA\tMBps/Kcp\tMBps/UDP\n"
              << "\t" << total_kcp_snd_pk << "\t" << total_udp_snd_pk << "\t" << total_snd_data << "\t" << total_udp_snd_data
              << "\t" << to_string_with_precision( ( (double)total_snd_data/ MB ) / diff ) << "\t" << to_string_with_precision( ( (double)total_udp_snd_data / MB ) / diff ) << "\n"
              << "\n\nRECV\tKCP\tUDP\tKCP_DATA\tUDP_DATA\tMBps/Kcp\tMBps/UDP\n"
              << "\t" << total_kcp_rcv_pk << "\t" << total_udp_rcv_pk << "\t" << total_rcv_data << "\t" << total_udp_rcv_data << "\t"
              << to_string_with_precision( ( (double)total_rcv_data / MB ) / diff ) << "\t" << to_string_with_precision( ( (double)total_udp_rcv_data / MB ) / diff ) << "\n";
    return 0;
}
