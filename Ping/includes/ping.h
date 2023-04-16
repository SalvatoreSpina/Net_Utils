#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <stdint.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>

#include "icmphdr.h"

// Buffer to receive ICMP packets
#define RECV_BUF_SIZE 1024

// A reasonable default TTL could be 64
// It allows the packet to traverse 64 routers before being discarded
#define DEFAULT_TTL 64

// A reasonable default packet size could be 64 bytes
// It's the size of a typical Ethernet packet
#define DEFAULT_PACKET_SIZE 56

// Simple linked list for storing round trip times
typedef struct trip_node_s
{
    double trip_time; // time in milliseconds
    struct trip_node_s *next;
} trip_node_t;

// Struct for storing ping flags, options and statistics
typedef struct
{
    int verbose;                    // enable verbose mode
    int quiet;                      // disable output messages
    int packet_count;               // number of packets to send
    unsigned long int time_to_live; // TTL (Time to Live) for packets
    unsigned long int packet_size;  // size of the packets to send
    unsigned long int interval;     // time between sending packets

    char *host; // hostname or IP address to ping

    int packets_sent;       // number of packets sent
    int packets_received;   // number of packets received
    double min_rtt;         // minimum round trip time
    double max_rtt;         // maximum round trip time
    double total_rtt;       // total round trip time
    trip_node_t *trip_list; // list of round trip times

    int socket;                        // socket file descriptor
    struct addrinfo *address;          // resolved address of the host
    char ip_address[INET6_ADDRSTRLEN]; // IP address of the host
    unsigned long int data_size;       // size of the data in packets
} ping_state_t;

extern ping_state_t global_ping;

// Ping functions
int print_usage(void);
void parse_args(int argc, const char **argv);
void initialize_network();
void statistics_signal_handler();
void ping_signal_handler();

// Utility functions

// Libft
unsigned long int atoull(const char *str);
double custom_sqrt(double x);
unsigned short calculate_checksum(void *data_ptr, size_t data_size);
unsigned short swap_endianess_16(unsigned short value);
void next_packet();

// Print utilities
void handle_icmp_error(unsigned short icmp_seq, icmphdr_t *received_packet);
void handle_error(unsigned short icmp_seq, const char *format, ...);