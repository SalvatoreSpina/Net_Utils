#include "ping.h"

// Global ping state
ping_state_t global_ping = {
    .verbose = 0,
    .time_to_live = DEFAULT_TTL,
    .packet_size = DEFAULT_PACKET_SIZE,
    .packet_count = -1,
    .quiet = 0,
    .interval = 1,

    .host = NULL,

    .packets_sent = 0,
    .packets_received = 0,
    .min_rtt = DBL_MAX,
    .max_rtt = 0,
    .total_rtt = 0,
    .trip_list = NULL,

    .socket = -1,
    .address = NULL,
    .ip_address = {0},
    .data_size = 0
};
