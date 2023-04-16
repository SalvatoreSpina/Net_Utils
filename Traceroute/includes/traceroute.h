#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/ip_icmp.h>

#include "icmphdr.h"

#define PACKET_SIZE 40
#define PACKET_DATA_SIZE (PACKET_SIZE - sizeof(struct icmphdr))
#define RECV_BUFSIZE 1024

// Default values for options
#define DEBUG false
#define DEFAULT_FIRST_TTL 1
#define DEFAULT_MAX_TTL 30
#define DEFAULT_PROBES_PER_TTL 3
#define DEFAULT_PACKET_TYPE 8

typedef struct
{
	char *target_host;			  // the host being targeted
	int debug;					  // flag to enable debugging output
	unsigned long first_ttl;	  // the initial time-to-live value for packets
	unsigned long max_ttl;		  // the maximum time-to-live value for packets
	unsigned long probes_per_ttl; // the number of probes sent per time-to-live value
	unsigned long packet_type;
} traceroute_options;

// Traceroute
void parse_options(int argc, char **argv, traceroute_options *options);
struct addrinfo *resolve_address(char *target_host);
int create_socket(struct addrinfo *addr, traceroute_options *options);
void trace_route(int sock, struct addrinfo *addr, const traceroute_options *options);

// Print utils
void print_help_text();
void handle_error(const char *error);
void print_trace_header(const char *target_host, const char *target_ip, unsigned long max_ttl);

// Utilities functions
struct timeval get_current_time();
unsigned short checksum(void *data_ptr, size_t data_size);
unsigned short swap_endianess_16(unsigned short value);
unsigned long int atoull(const char *s);
unsigned short calculate_checksum(void *data_ptr, size_t data_size);
bool memory_regions_differ(const void *first_region, const void *second_region, size_t len);
bool strings_equal(const char *first_region, const char *second_region);