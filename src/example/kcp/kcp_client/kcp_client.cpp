#include <signal.h>
#include <thread>
#include <chrono>
#include <atomic>

#include "../include/util.h"
#include "../include/client.h"
#include "../include/joining_thread.h"

constexpr auto default_ip = "127.0.0.1";
constexpr auto default_port = 9527;
constexpr auto default_max_len = 2000;
constexpr auto default_test_times = 1000;
constexpr auto default_lost_rate = 0;
constexpr auto default_send_interval = 50; // ms

bool g_running = true;
std::atomic<uint32_t> total_kcp_snd_pk {}; // total kcp pack
std::atomic<uint32_t> total_udp_snd_pk {}; // total upd pack
std::atomic<uint64_t> total_udp_snd_data {};
std::atomic<uint64_t> total_snd_data {};   // total data size

std::atomic<uint32_t> total_kcp_rcv_pk {}; // total kcp pack
std::atomic<uint32_t> total_udp_rcv_pk {}; // total upd pack
std::atomic<uint64_t> total_udp_rcv_data {};
std::atomic<uint64_t> total_rcv_data {};   // total data size

std::atomic<uint32_t> finished_cnt {};

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

int main( int argc, char ** argv )
{
    handle_signal();

    int mode = 0;
    std::string ip = default_ip;
    uint16_t port = default_port;
    int max_len = default_max_len;
    int test_times = default_test_times;
    int lost_rate = default_lost_rate;
    int send_interval = default_send_interval;

    if ( argc >= 2 ) {
        ip = argv[1];
    }

    if ( argc >= 3 ) {
        port = atoi( argv[2] );
    }

    if ( argc >= 4 ) {
        mode = atoi( argv[3] );
    }

    if ( argc >= 5 ) {
        max_len = atoi( argv[4] );
    }

    if ( argc >= 6 ) {
        test_times = atoi( argv[5] );
    }

    if ( argc >= 7 ) {
        lost_rate = atoi( argv[6] );
    }

    if ( argc >= 8 ) {
        send_interval = atoi( argv[7] );
    }

    printf( "Usage:<%s> <ip>:%s <port>:%d  <mode>:%s <max_len>:%d <times>:%d <lost_rate>:%d <send_interval>:%dms\n",
        argv[0],
        ip.c_str(),
        port,
        util::get_mode_name( mode ),
        max_len,
        test_times,
        lost_rate,
        send_interval );


    using namespace std::chrono;
    auto now = duration_cast<milliseconds>( system_clock::now().time_since_epoch() ).count();

    std::unique_ptr<Client> client = std::make_unique<Client>( ip.c_str(), port, conv );
    client->setmode( mode );
    client->setauto( true, test_times, max_len );
    client->setlostrate( lost_rate );
    client->setsendinterval( send_interval );
    client->set_show_info( true );
    //  client->set_show_detail(true);
    //  client->set_delay( true );

    joining_thread work( &Client::loop, client.get());
    joining_thread input( &Client::input, client.get() );

    while ( g_running ) {
        if ( finished_cnt == 1 ) {
            g_running = false;
            break;
        }

        std::this_thread::sleep_for( std::chrono::milliseconds { 1 } );
    }

    client->terminate();
    auto diff = duration_cast<milliseconds>( system_clock::now().time_since_epoch() ).count() - now;
    std::cout << " total spend time " << diff << " millisecond\n";
 
    // TOTAL REPORT   
    diff /=1000.0f;
    using namespace util;
    std::cout << "\nSEND\tKCP\tUDP\tKCP_DATA\tUDP_DATA\tMBps/Kcp\tMBps/UDP\n"
              << "\t" << total_kcp_snd_pk << "\t" << total_udp_snd_pk << "\t" << total_snd_data << "\t" << total_udp_snd_data
              << "\t" << to_string_with_precision( ( (double)total_snd_data/ MB ) / diff ) << "\t" << to_string_with_precision( ( (double)total_udp_snd_data / MB ) / diff ) << "\n"
              << "\n\nRECV\tKCP\tUDP\tKCP_DATA\tUDP_DATA\tMBps/Kcp\tMBps/UDP\n"
              << "\t" << total_kcp_rcv_pk << "\t" << total_udp_rcv_pk << "\t" << total_rcv_data << "\t" << total_udp_rcv_data << "\t"
              << to_string_with_precision( ( (double)total_rcv_data / MB ) / diff ) << "\t" << to_string_with_precision( ( (double)total_udp_rcv_data / MB ) / diff ) << "\n";


    return 0;
}
