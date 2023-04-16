#include "traceroute.h"

int main(int argc, char **argv)
{
    // Set default values for options
    traceroute_options options = {
        .target_host = NULL,
        .debug = DEBUG,
        .first_ttl = DEFAULT_FIRST_TTL,
        .max_ttl = DEFAULT_MAX_TTL,
        .probes_per_ttl = DEFAULT_PROBES_PER_TTL,
        .packet_type = DEFAULT_PACKET_TYPE
    };

    // Parse command line arguments
    parse_options(argc, argv, &options);

    // Resolve hostname to IP address
    struct addrinfo *addr = resolve_address(options.target_host);

    // Create socket for sending/receiving ICMP packets
    int sock = create_socket(addr, &options);

    // Get the IP address of the host
    char hostip_s[INET6_ADDRSTRLEN];
    inet_ntop(addr->ai_family, &((struct sockaddr_in *)addr->ai_addr)->sin_addr, hostip_s, INET6_ADDRSTRLEN);

    // Print the trace header
    print_trace_header(options.target_host, hostip_s, options.max_ttl);

    // Start the trace route
    trace_route(sock, addr, &options);

    // Clean up
    freeaddrinfo(addr);

    // Close socket
    close(sock);

    // Exit
    return EXIT_SUCCESS;
}