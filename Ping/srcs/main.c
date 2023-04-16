#include "ping.h"

int main(const int argc, const char **argv)
{
    // parse command line arguments
    parse_args(argc, argv);

    // initialize network [address, socket, etc.]
    initialize_network();

    // print ping header
    printf("PING %s (%s): %lu data bytes\n", global_ping.host, global_ping.ip_address, global_ping.packet_size);

    // set signal handlers
    signal(SIGINT, (void (*)())statistics_signal_handler);
    signal(SIGALRM, (void (*)())ping_signal_handler);

    // start pinging
    ping_signal_handler();

    while ("pinging")
        ; // ping
}