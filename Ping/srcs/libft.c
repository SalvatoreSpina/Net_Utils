#include "ping.h"

// This function checks if a character is a digit.
// @param c The character to check.
// @return 1 if the character is a digit, 0 otherwise.
bool is_digit(const int c) { return (c >= '0' && c <= '9'); }

// This function converts a string to an unsigned long int .
// @param s The string to convert.
// @return The converted unsigned long int .
unsigned long int atoull(const char *s)
{
    if (!is_digit(*s))
    {
        fprintf(stderr, "ping: expected u8\n");
        exit(1);
    }
    unsigned long int n = 0;
    while (is_digit(*s))
    {
        n = n * 10 + *s - '0';
        ++s;
    }
    return (n);
}

// This function swaps the endianness of a 16-bit unsigned integer.
// @param It takes an argument called "value"
// @return a new 16-bit unsigned integer with its bytes swapped.
unsigned short swap_endianess_16(unsigned short value)
{
    unsigned short msb = (value & 0xff) << 8; // Most significant byte
    unsigned short lsb = value >> 8;          // Least significant byte
    return msb | lsb;                         // Bitwise OR of the two bytes
}

// Calculates the checksum of a given data buffer using the Internet checksum algorithm.
// @param data_ptr: pointer to the start of the data buffer
// @param data_size: size of the data buffer
// @return the calculated checksum as an unsigned short
uint16_t calculate_checksum(void *data_ptr, size_t data_size)
{
    uint16_t *data = data_ptr;
    uint64_t sum = 0;

    // Sum up the uint16_t values in the data block
    while (data_size >= sizeof(*data))
    {
        sum += *data++;
        data_size -= sizeof(*data);
    }

    // If there is any remaining data, add it to the sum as a uint8_t value
    if (data_size)
    {
        sum += *(uint8_t *)data;
    }

    // Fold the sum into a 16-bit value
    while (sum & ~0xffff)
    {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    return (~sum);
}

// Custom implementation of the sqrt function.
// @param x The number to compute the square root of.
// @return The square root of x.
double custom_sqrt(double x)
{
    if (x == 0.0)
    {
        return 0.0;
    }

    // Initialize variables to store previous and current estimates of the square root
    double prev = 0.0;
    double curr = 1.0;

    // Iterate until the current and previous estimates are the same
    while (prev != curr)
    {
        // Update the previous estimate to the current estimate
        prev = curr;
        // Compute the new estimate of the square root using the Newton-Raphson method
        curr = 0.5 * (prev + x / prev);
    }

    return curr;
}

// Set an alarm for the next packet to be sent, and exit if all packets have been sent.
// @param None.
// @return None.
void next_packet()
{
    if (global_ping.packet_count == global_ping.packets_sent)
    {
        statistics_signal_handler();
        exit(EXIT_SUCCESS);
    }
    alarm(global_ping.interval);
}