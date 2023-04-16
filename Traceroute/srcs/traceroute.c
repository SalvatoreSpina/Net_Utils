#include "traceroute.h"

// Creates an ICMP packet for ping with given sequence number.
// @param packet A pointer to the packet structure to be filled.
// @param sequence_number The sequence number to be used in the packet.
static void create_packet(icmphdr_t *packet, const traceroute_options *options)
{
    // Set packet header fields
    packet->type = options->packet_type;
    packet->code = 0;
    packet->checksum = 0;
    packet->un.echo.id = swap_endianess_16(getpid());
    packet->un.echo.sequence = swap_endianess_16(1);

    // Fill packet with data
    for (unsigned long int i = 0; i < PACKET_DATA_SIZE; ++i)
    {
        ((char *)packet)[sizeof(icmphdr_t) + i] = 'a' + i % 26;
    }

    // Compute and set packet checksum
    packet->checksum = calculate_checksum(packet, PACKET_SIZE);
}

static bool send_probes(int sock, struct addrinfo *addr, const traceroute_options *options)
{
    // Initialize a flag to indicate if the destination has been reached
    bool reached = true;

    // Initialize a struct to hold the address of the previous hop
    struct sockaddr_in prev_addr = {};

    // Loop for the specified number of probes per hop
    for (unsigned long i = 0; i < options->probes_per_ttl; ++i)
    {
        // Create a buffer to hold the traceroute packet
        char buf[PACKET_SIZE];

        // Craft a traceroute packet and store it in the buffer
        create_packet((icmphdr_t *)buf, options);

        // Get the current time for measuring round-trip time
        struct timeval start = get_current_time();

        // Send the packet to the destination host
        if (sendto(sock, buf, PACKET_SIZE, 0, addr->ai_addr, addr->ai_addrlen) < 0)
        {
            // Print an error message and exit if the call fails
            perror("traceroute: sendto");
            exit(EXIT_FAILURE);
        }

        // Receive a response from the destination host
        char recvbuf[RECV_BUFSIZE];
        struct sockaddr_in r_addr;
        unsigned int addr_len = sizeof(r_addr);
        if (recvfrom(sock, &recvbuf, RECV_BUFSIZE, 0, (struct sockaddr *)&r_addr, &addr_len) < 0)
        {
            // Print a * to indicate that no response was received
            printf("  *");
            // Set the reached flag to false
            reached = false;
            // Continue to the next iteration of the loop
            continue;
        }

        // Get the current time to calculate round-trip time
        struct timeval end = get_current_time();
        double time = (double)(end.tv_sec - start.tv_sec) * 1000 + (double)(end.tv_usec - start.tv_usec) / 1000;

        // Convert the responding host's IP address to a string
        char ipbuf[INET6_ADDRSTRLEN];
        inet_ntop(r_addr.sin_family, &r_addr.sin_addr, ipbuf, sizeof(ipbuf));

        // Print the responding host's IP address if it differs from the previous hop
        if (!i || memory_regions_differ(&prev_addr, &r_addr, sizeof(r_addr)))
        {
            printf(" %s", ipbuf);
        }

        // Print the round-trip time to the console
        printf(" %.3f ms", time);

        // Store the address of the current hop as the previous hop
        prev_addr = r_addr;

        // Check the type of the ICMP response packet
        icmphdr_t *icmp_res = (void *)recvbuf + sizeof(struct ip);
        if (icmp_res->type == ICMP_TIMXCEED || icmp_res->type == ICMP_UNREACH_PORT)
        {
            // If the response is a "time exceeded" or "port unreachable" message,
            // the destination has not been reached
            reached = false;
        }
    }

    return reached;
}

void trace_route(int sock, struct addrinfo *addr, const traceroute_options *options)
{
    // Initialize the number of hops to the first TTL value
    unsigned long hops = options->first_ttl;

    // Loop until the maximum TTL value is reached
    while (hops <= options->max_ttl)
    {
        // Set the IP TTL option for the socket
        if (setsockopt(sock, IPPROTO_IP, IP_TTL, &hops, sizeof(hops)) < 0)
        {
            // Print an error message and exit if the call fails
            perror("traceroute: setsockopt IP_TTL");
            exit(EXIT_FAILURE);
        }

        // Print the current hop count to the console
        printf("%2ld", hops);

        // Send probes
        bool reached = send_probes(sock, addr, options);

        // Print a newline character to the console after all probes for this hop have been sent
        printf("\n");

        // If the destination has been reached, exit the loop
        if (reached)
        {
            break;
        }

        // Increment the hop count and continue to the next iteration of the loop
        ++hops;
    }
}