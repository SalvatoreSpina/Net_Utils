#include "ping.h"

// This function is called when the program receives a SIGINT signal.
// It prints the statistics of the ping.
void statistics_signal_handler()
{
    printf("\n");
    printf("--- %s ft_ping statistics ---\n", global_ping.host);

    // Calculate packet loss percentage
    float packet_loss_percentage = 100.0 * (1 - (float)global_ping.packets_received / global_ping.packets_sent);

    printf("%d packets transmitted, %d packets received, %.1f%% packet loss\n",
           global_ping.packets_sent,
           global_ping.packets_received,
           packet_loss_percentage);

    // If no packets were received, exit with error
    if (!global_ping.packets_received)
    {
        exit(EXIT_FAILURE);
    }

    // Calculate the average round trip time
    double round_trip_time_average = global_ping.total_rtt / global_ping.packets_received;

    // Calculate the standard deviation of the round trip time using Welford's algorithm
    double round_trip_time_variance = 0;
    double round_trip_time_stddev = 0;
    for (trip_node_t *it = global_ping.trip_list; it != NULL; it = it->next)
    {
        double deviation = it->trip_time - round_trip_time_average;
        round_trip_time_variance += deviation * deviation * ((double)it->trip_time - 1) / it->trip_time;
    }
    if (global_ping.packets_received > 1)
    {
        round_trip_time_stddev = custom_sqrt(round_trip_time_variance / (global_ping.packets_received - 1));
    }

    // Print the minimum, average, maximum, and standard deviation of the round trip time
    printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", global_ping.min_rtt, round_trip_time_average, global_ping.max_rtt, round_trip_time_stddev);

    // Exit successfully
    exit(EXIT_SUCCESS);
}

// @brief Checks the received ICMP packet against expected values.
// @param received_packet Pointer to the received ICMP packet to be checked.
// @param packet_length The length of the received packet.
// @param icmp_seq The sequence number of the ICMP packet.
// @param received_checksum The checksum of the received packet.
// @param buffer The buffer used to send the ICMP packet.
// @return Returns true if the received ICMP packet passes all checks, otherwise false.
bool check_packet(icmphdr_t *received_packet, ssize_t packet_length, unsigned short icmp_seq, unsigned short received_checksum, char *buffer)
{
    if (received_checksum != calculate_checksum(received_packet, packet_length - sizeof(struct ip)))
    {
        handle_error(icmp_seq, "Invalid checksum");
        return false;
    }

    // Check if the received packet is an echo reply
    if (received_packet->type != ICMP_ECHOREPLY)
    {
        handle_icmp_error(icmp_seq, received_packet);
        return false;
    }

    // Check if the received packet has a valid code
    if (received_packet->code != 0)
    {
        handle_error(icmp_seq, "Invalid ICMP code (%d)", received_packet->code);
        return false;
    }

    // Check if the received packet has the expected ID
    if (received_packet->un.echo.id != ((icmphdr_t *)buffer)->un.echo.id)
    {
        handle_error(icmp_seq, "Wrong ID (%d)", received_packet->un.echo.id);
        return false;
    }

    // Check if the received packet has the expected size
    if (packet_length < (ssize_t)(sizeof(struct ip) + global_ping.data_size))
    {
        handle_error(icmp_seq, "Packet content is missing");
        return false;
    }

    for (size_t i = 0; i < global_ping.packet_size; ++i)
    {
        if (((char *)received_packet + sizeof(icmphdr_t))[i] != (buffer + sizeof(icmphdr_t))[i])
        {
            handle_error(icmp_seq, "Not same content");
            return false;
        }
    }

    return true;
}

// Creates an ICMP packet for ping with given sequence number.
// @param packet A pointer to the packet structure to be filled.
// @param sequence_number The sequence number to be used in the packet.
void create_packet(icmphdr_t *packet, unsigned short sequence_number)
{
    // Set packet header fields
    packet->type = ICMP_ECHO;
    packet->code = 0;
    packet->checksum = 0;
    packet->un.echo.id = swap_endianess_16(getpid());
    packet->un.echo.sequence = swap_endianess_16(sequence_number);

    // Fill packet with data
    for (unsigned long int i = 0; i < global_ping.packet_size; ++i)
    {
        ((char *)packet)[sizeof(icmphdr_t) + i] = 'a' + i % 26;
    }

    // Compute and set packet checksum
    packet->checksum = calculate_checksum(packet, global_ping.data_size);
}

// Calculate round trip time and update statistics
// @param start: start time of the ping
// @param end: end time of the ping
// @return round trip time in ms
double calculate_round_trip_time(struct timeval start, struct timeval end)
{
    double time_ms = (double)(end.tv_sec - start.tv_sec) * 1000 + (double)(end.tv_usec - start.tv_usec) / 1000;

    // Update minimum and maximum round trip times
    global_ping.min_rtt = global_ping.min_rtt < time_ms ? global_ping.min_rtt : time_ms;
    global_ping.max_rtt = global_ping.max_rtt > time_ms ? global_ping.max_rtt : time_ms;

    // Update total round trip time
    global_ping.total_rtt += time_ms;

    // Return round trip time in ms
    return time_ms;
}

// Add round trip time to linked list of trips
// @param time_ms: round trip time in ms
void add_trip_to_list(const double time_ms)
{
    trip_node_t *node = malloc(sizeof(trip_node_t));
    if (node == NULL)
    {
        handle_error(0, "Failed to allocate memory for trip list");
        exit(EXIT_FAILURE);
    }
    node->trip_time = time_ms;
    node->next = global_ping.trip_list;
    global_ping.trip_list = node;
}

// Signal handler function for sending and receiving ICMP packets, and handling their responses.
// The function sends an ICMP packet, waits for a response, checks if the response is valid,
// calculates the round trip time, adds it to the list, updates packet statistics, and prints
// the ping reply if quiet mode is disabled.
// Define the function `ping_signal_handler()`
void ping_signal_handler(void)
{
    // Define the ICMP sequence number as the number of packets sent
    unsigned short icmp_seq = global_ping.packets_sent;

    // Create a buffer to hold the packet data
    char buffer[global_ping.data_size];

    // Create the packet with the given sequence number
    create_packet((icmphdr_t *)buffer, icmp_seq);

    // Create a buffer to receive the response
    char recv_buffer[RECV_BUF_SIZE];

    // Define a struct iovec that points to the receive buffer and has a size of `RECV_BUF_SIZE`
    struct iovec iov = {recv_buffer, sizeof(recv_buffer)};

    // Define a struct msghdr with default values
    struct msghdr msg = {0};

    // Set the msg_iov field to point to the iov array and set the length of the array to 1
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // Define start and end times for measuring the round trip time
    struct timeval start_time, end_time;

    // Get the current time to use as the start time
    if (gettimeofday(&start_time, NULL) < 0)
    {
        // Print an error message and exit if an error occurs
        perror("ping: gettimeofday");
        exit(1);
    }

    // Send the packet
    sendto(
        global_ping.socket,             // socket file descriptor
        buffer,                         // data buffer containing the packet
        global_ping.data_size,          // size of the packet data
        0,                              // flags (none)
        global_ping.address->ai_addr,   // destination address
        global_ping.address->ai_addrlen // length of the destination address
    );

    // Increment the number of packets sent
    ++global_ping.packets_sent;

    // Receive the response
    ssize_t recv_size = recvmsg(global_ping.socket, &msg, 0);

    // Get the current time to use as the end time
    if (gettimeofday(&end_time, NULL) < 0)
    {
        // Print an error message and exit if an error occurs
        perror("ping: gettimeofday");
        exit(1);
    }

    // Print an error message and exit if an error occurs during receiving
    if (recv_size < 0)
    {
        perror("ping: recvmsg");
        exit(1);
    }

    // Get a pointer to the received ICMP packet
    icmphdr_t *received_packet = (icmphdr_t *)(recv_buffer + sizeof(struct ip));

    // Get the received checksum
    unsigned short received_checksum = received_packet->checksum;

    // Set the received checksum to 0 to calculate it later
    received_packet->checksum = 0;

    // Check if the received packet is valid
    const bool is_received = check_packet(received_packet, recv_size, icmp_seq, received_checksum, buffer);

    // Calculate the round trip time
    const double trip_time = calculate_round_trip_time(start_time, end_time);

    // Add the round trip time to the list
    add_trip_to_list(trip_time);

    // Increment the number of packets received if the packet is valid
    global_ping.packets_received += is_received;

    // Print the ping reply if quiet mode is disabled
    if (!global_ping.quiet)
    {
        printf("%zd bytes from %s: icmp_seq=%d ttl=%lu time=%.3f ms\n", recv_size, global_ping.ip_address, icmp_seq, global_ping.time_to_live, trip_time);
    }

    next_packet();
}