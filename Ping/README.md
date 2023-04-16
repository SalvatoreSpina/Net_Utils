# Ping

## Description

This repository contains a C program that implements a ping utility for testing network connectivity. The program sends ICMP echo request packets to a specified host and measures the round-trip time of the packets to determine the network latency.

## Usage

To use the ping utility, run the following command in a terminal:

``` bash
./ping [hostname]
```

Replace `host` with the IP address or domain name of the host to ping. The available options are:

- `q Quiet mode`: only display output at start and when finished.
- `v Verbose mode`: display additional output.
- `-c Count`: Stop after sending and receiving `count` packets.
- `-s Size`: Set the packet size to `size` bytes.
- `i Interval`: Wait interval seconds between sending each packet.
- `t TTL`: Set the TTL (Time To Live) value of the packets.
