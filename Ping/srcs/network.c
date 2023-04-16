#include "ping.h"

// The init_network function is responsible for setting up the network connection
// and socket options needed for sending and receiving ICMP packets.
void initialize_network()
{
    // Create an addrinfo structure with criteria for the address lookup
    struct addrinfo address_hints = {0};
    address_hints.ai_family = AF_INET;        // IPv4 addresses only
    address_hints.ai_socktype = SOCK_RAW;     // raw IP packets
    address_hints.ai_protocol = IPPROTO_ICMP; // use the ICMP protocol

    // Resolve the IP address of the target host
    int address_lookup_result = getaddrinfo(global_ping.host, NULL, &address_hints, &global_ping.address);
    if (address_lookup_result < 0)
    {
        fprintf(stderr, "ping: cannot resolve %s: Unknown host\n", global_ping.host);
        exit(1);
    }

    // Create a socket for sending and receiving ICMP packets
    global_ping.socket = socket(global_ping.address->ai_family, SOCK_RAW, IPPROTO_ICMP);
    if (global_ping.socket < 0)
    {
        perror("ping: socket");
        exit(1);
    }

    // Set the TTL (time to live) for the packets to the value specified by the user
    int ttl_socket_option = global_ping.time_to_live;
    int set_ttl_result = setsockopt(global_ping.socket, IPPROTO_IP, IP_TTL, &ttl_socket_option, sizeof(ttl_socket_option));
    if (set_ttl_result < 0)
    {
        perror("ping: setsockopt IP_TTL");
        exit(1);
    }

    // Set a timeout for receiving packets
    struct timeval receive_timeout = {1, 0}; // 1 second timeout
    int set_receive_timeout_result = setsockopt(global_ping.socket, SOL_SOCKET, SO_RCVTIMEO, &receive_timeout, sizeof(receive_timeout));
    if (set_receive_timeout_result < 0)
    {
        perror("ping: setsockopt SO_RCVTIMEO");
        exit(1);
    }

    // Convert the IP address of the target host to a string representation
    const char *ip_address_ptr = inet_ntop(global_ping.address->ai_family, &((struct sockaddr_in *)global_ping.address->ai_addr)->sin_addr, global_ping.ip_address, INET6_ADDRSTRLEN);
    if (ip_address_ptr == NULL)
    {
        perror("ping: inet_ntop");
        exit(1);
    }

    // Calculate the total size of the data in each ICMP packet
    global_ping.data_size = global_ping.packet_size + sizeof(icmphdr_t);
}
