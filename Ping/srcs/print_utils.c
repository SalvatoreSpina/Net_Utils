#include "ping.h"

// This function print the usage of the program
// @param program_name The name of the program
// @return EXIT_FAILURE
int print_usage(void)
{
	fprintf(stderr, "Usage: ft_ping [OPTIONS] HOST\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Mandatory:\n");
	fprintf(stderr, "    -v             Verbose output\n");
	fprintf(stderr, "    -h             Show help\n");

	fprintf(stderr, "Bonuses:\n");
	fprintf(stderr, "    -q             Quiet, only display output at start/finish\n");
	fprintf(stderr, "    -c COUNT       Send only COUNT pings\n");
	fprintf(stderr, "    -t TTL         Set Time To Live (default %d)\n", DEFAULT_TTL);
	fprintf(stderr, "    -s SIZE        Send SIZE data bytes in packets (default %d)\n", DEFAULT_PACKET_SIZE);
	fprintf(stderr, "    -i SECS        Interval (default 1)\n");
	return (EXIT_FAILURE);
}

// This function handles the errors
// @param icmp_seq The icmp sequence number
// @param fmt The format of the error
// @param ... The arguments
// @return void
void handle_error(unsigned short icmp_seq, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	if (!global_ping.quiet && global_ping.verbose)
	{
		printf("From %s: icmp_seq=%d ", global_ping.ip_address, icmp_seq);
		vprintf(format, args);
		printf("\n");
	}
	va_end(args);
	next_packet();
	return;
}

// Print the icmp error
// @param icmp_seq The icmp sequence number
// @param received_packet The received packet
// @return void
void handle_icmp_error(unsigned short icmp_seq, icmphdr_t *received_packet)
{
	switch (received_packet->type)
	{
	case ICMP_DEST_UNREACH:
		switch (received_packet->code)
		{
		case ICMP_NET_UNREACH:
			handle_error(icmp_seq, "Net Unreachable");
			break;
		case ICMP_HOST_UNREACH:
			handle_error(icmp_seq, "Host Unreachable");
			break;
		case ICMP_PROT_UNREACH:
			handle_error(icmp_seq, "Protocol Unreachable");
			break;
		case ICMP_PORT_UNREACH:
			handle_error(icmp_seq, "Port Unreachable");
			break;
		case ICMP_FRAG_NEEDED:
			handle_error(icmp_seq, "Fragmentation Needed and Don't Fragment was Set");
			break;
		case ICMP_SR_FAILED:
			handle_error(icmp_seq, "Source Route Failed");
			break;
		case ICMP_NET_UNKNOWN:
			handle_error(icmp_seq, "Destination Network Unknown");
			break;
		case ICMP_HOST_UNKNOWN:
			handle_error(icmp_seq, "Destination Host Unknown");
			break;
		case ICMP_HOST_ISOLATED:
			handle_error(icmp_seq, "Source Host Isolated");
			break;
		case ICMP_NET_ANO:
			handle_error(icmp_seq, "Communication with Destination Network is Administratively Prohibited");
			break;
		case ICMP_HOST_ANO:
			handle_error(icmp_seq, "Communication with Destination Host is Administratively Prohibited");
			break;
		case ICMP_NET_UNR_TOS:
			handle_error(icmp_seq, "Destination Network Unreachable for Type of Service");
			break;
		case ICMP_HOST_UNR_TOS:
			handle_error(icmp_seq, "Destination Host Unreachable for Type of Service");
			break;
		case ICMP_PKT_FILTERED:
			handle_error(icmp_seq, "Communication Administratively Prohibited");
			break;
		case ICMP_PREC_VIOLATION:
			handle_error(icmp_seq, "Host Precedence Violation");
			break;
		case ICMP_PREC_CUTOFF:
			handle_error(icmp_seq, "Precedence cutoff in effect");
			break;
		default:
			handle_error(icmp_seq, "Destination unreachable");
			break;
		}
		break;

	case ICMP_SOURCE_QUENCH:
		handle_error(icmp_seq, "Source Quench");
		break;

	case ICMP_REDIRECT:
		switch (received_packet->code)
		{
		case ICMP_REDIR_NET:
			handle_error(icmp_seq, "Redirect for Destination Network");
			break;
		case ICMP_REDIR_HOST:
			handle_error(icmp_seq, "Redirect for Destination Host");
			break;
		case ICMP_REDIR_NETTOS:
			handle_error(icmp_seq, "Redirect for Destination Network Based on Type-of-Service");
			break;
		case ICMP_REDIR_HOSTTOS:
			handle_error(icmp_seq, "Redirect for Destination Host Based on Type-of-Service");
			break;
		default:
			handle_error(icmp_seq, "Redirect");
			break;
		}
		break;

	case ICMP_TIME_EXCEEDED:
		switch (received_packet->code)
		{
		case ICMP_EXC_TTL:
			handle_error(icmp_seq, "Time-to-Live Exceeded in Transit");
			break;
		case ICMP_EXC_FRAGTIME:
			handle_error(icmp_seq, "Fragment Reassembly Time Exceeded");
			break;
		default:
			handle_error(icmp_seq, "Time Exceeded");
			break;
		}
		break;

	case ICMP_PARAMETERPROB:
		switch (received_packet->code)
		{
		case ICMP_ERRATPTR:
			handle_error(icmp_seq, "Pointer indicates the error");
			break;
		case ICMP_OPTABSENT:
			handle_error(icmp_seq, "Missing a Required Option");
			break;
		case ICMP_BAD_LENGTH:
			handle_error(icmp_seq, "Bad Length");
			break;
		default:
			handle_error(icmp_seq, "Parameter Problem");
			break;
		}
		break;

	default:
		handle_error(icmp_seq, "Unknown Error");
		break;
	}
}