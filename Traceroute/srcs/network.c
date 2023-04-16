#include "traceroute.h"

// Resolves a hostname to an IP address using the getaddrinfo function.
// @param target_host the hostname to resolve
// @return a pointer to the first addrinfo structure in a linked list, or NULL if an error occurs
struct addrinfo *resolve_address(char *target_host)
{
	// Set up hints for getaddrinfo
	struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_RAW,
		.ai_protocol = IPPROTO_ICMP
	};
	struct addrinfo *addr = NULL;

	// Call getaddrinfo to resolve the hostname to an IP address
	if (getaddrinfo(target_host, NULL, &hints, &addr) != 0)
	{
		// If an error occurs, print an error message and exit
		fprintf(stderr, "traceroute: cannot resolve %s: Unknown host\n", target_host);
		exit(EXIT_FAILURE);
	}

	return addr;
}

// Creates a raw socket for sending and receiving ICMP packets.
// @param addr a pointer to the addrinfo structure containing the destination IP address and port
// @param options a pointer to the traceroute_options struct containing program options
// @return the file descriptor of the created socket, or -1 if an error occurs
int create_socket(struct addrinfo *addr, traceroute_options *options)
{
	// Create a raw socket for sending and receiving ICMP packets
	int sock = socket(addr->ai_family, SOCK_RAW, IPPROTO_ICMP);
	if (sock < 0)
	{
		perror("traceroute: could not create socket");
		exit(EXIT_FAILURE);
	}

	// Set socket timeout for receiving packets
	struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) != 0)
	{
		perror("traceroute: setsockopt SO_RCVTIMEO");
		exit(EXIT_FAILURE);
	}

	// Enable debug mode if requested
	if (options->debug && setsockopt(sock, SOL_SOCKET, SO_DEBUG, &options->debug, sizeof(options->debug)))
	{
		perror("traceroute: setsockopt SO_DEBUG");
		exit(EXIT_FAILURE);
	}

	return sock;
}
