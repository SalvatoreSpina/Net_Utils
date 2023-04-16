# Traceroute

This project is a C implementation of the Traceroute utility, which is used to determine the path packets take from one host to another over an IP network. It uses the Internet Control Message Protocol (ICMP) to identify the routers along the path.

## Usage

### Arguments

- `host`: The host to traceroute to

### Options

- `--help`: Read the help and exit
- `-d`: Enable socket level debugging
- `-f first_ttl`: Start from the first_ttl hop (instead from 1)
- `-m max_ttl`: Set the max number of hops (max TTL to be reached). Default is 30
- `-q nqueries`: Set the number of probes per each hop. Default is 3
- `-I`: Use ICMP ECHO for tracerouting

## How it works

This implementation sends packets with increasing TTL (Time-to-Live) values, starting from 1. Each router along the path decrements the TTL value by 1. When a router decrements the TTL to 0, it discards the packet and sends an ICMP Time Exceeded message back to the source host. This allows the source host to identify the router.

If the destination host is reached, it will send an ICMP Echo Reply message back to the source host, indicating that the path has been successfully traced.

This implementation sends multiple packets to each router to get more accurate results.
