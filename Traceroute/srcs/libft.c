#include "traceroute.h"

// This function checks if a character is a digit.
// @param c The character to check.
// @return 1 if the character is a digit, 0 otherwise.
static bool is_digit(const int c) { return (c >= '0' && c <= '9'); }

// This function converts a string to an unsigned long int .
// @param s The string to convert.
// @return The converted unsigned long int .
unsigned long int atoull(const char *s)
{
    if (!is_digit(*s))
    {
        fprintf(stderr, "traceroute: atoull\n");
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

// @brief Get the current time as a timeval struct.
// @return The current time as a timeval struct.
struct timeval get_current_time()
{
    struct timeval time;
    if (gettimeofday(&time, NULL) < 0)
    {
        perror("get_current_time: gettimeofday");
        exit(EXIT_FAILURE);
    }
    return time;
}

// @brief Compare two memory regions and return 1 if they differ, 0 otherwise.
// @param a A pointer to the first memory region.
// @param b A pointer to the second memory region.
// @param len The length of the memory regions.
// @return 1 if the memory regions differ, 0 otherwise.
bool memory_regions_differ(const void *first_region, const void *second_region, size_t len)
{
    for (size_t i = 0; i < len; ++i)
    {
        if (((unsigned char *)first_region)[i] != ((unsigned char *)second_region)[i])
        {
            return 1;
        }
    }
    return false;
}

// @brief Count the length of a string.
// @param s A pointer to the string.
// @return The length of the string.
static int ft_strlen(const char *s)
{
    int len = 0;
    while (s[len] != '\0')
    {
        ++len;
    }
    return len;
}

// @brief Check if two strings are equal.
// @param s A pointer to the first string.
// @param const_s A pointer to the second string.
// @return 1 if the strings are equal, 0 otherwise.
bool strings_equal(const char *first_region, const char *second_region)
{
    return memory_regions_differ(first_region, second_region, ft_strlen(first_region) + 1) == 0;
}
