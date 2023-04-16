#include "traceroute.h"

// Prints the trace route header.
// @param target_host The target host name or IP address.
// @param hostip_s The IP address of the target host.
// @param max_hops The maximum number of hops to reach the target.
void print_trace_header(const char *target_host, const char *hostip_s, unsigned long max_hops)
{
    printf("traceroute to %s (%s), %ld hops max, %ld byte packets\n",
           target_host, hostip_s,
           max_hops, PACKET_SIZE + sizeof(struct ip));
}

// @brief Print an error message and exit.
// @param s A pointer to the error message.
void handle_error(const char *error)
{
    fprintf(stderr, "traceroute: %s\n", error);
    exit(EXIT_FAILURE);
}

// @brief Print the help text for the traceroute program.
void print_help_text()
{
    printf("Usage:\n");
    printf("  ./traceroute host\n");
    printf("Mandatory Options:\n");
    printf("  --help      Print this help text and exit.\n");
    printf("Bonus Options:\n");
    printf("  -4          Use IPv4.\n");
    printf("  -I          Use ICMP ECHO for tracerouting.\n");
    printf("  -d          Enable socket level debugging.\n");
    printf("  -f first_ttl\n");
    printf("      Start from the specified first_ttl hop (instead of 1).\n");
    printf("  -m max_ttl\n");
    printf("      Set the maximum number of hops (max TTL to be reached). Default is 30.\n");
    printf("  -q nqueries\n");
    printf("      Set the number of probes per hop. Default is 3.\n");
    printf("\n");
    printf("Arguments:\n");
    printf("  host        The host to traceroute.\n");
}
