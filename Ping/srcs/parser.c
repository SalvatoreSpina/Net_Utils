#include "ping.h"

// Checks if the next argument exists in the command-line arguments and increments the index.
// Exits the program with an error message if the next argument does not exist.
// @param argc An integer representing the number of arguments passed to the program.
// @param i A pointer to an integer representing the current index of the command-line arguments.
static void check_next_arg(const int argc, int *i)
{
    if (++*i >= argc)
    {
        exit(print_usage());
    }
}

// Parses command-line arguments and sets the corresponding options in the global_ping struct.
// @param argc An integer representing the number of arguments passed to the program.
// @param argv An array of strings representing the arguments passed to the program.
void parse_args(int argc, const char **argv)
{
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            if (argv[i][1] == '\0' || argv[i][2] != '\0')
                exit(print_usage());
            if (argv[i][1] == 'v')
                global_ping.verbose = 1;
            else if (argv[i][1] == 'q')
                global_ping.quiet = 1;
            else if (argv[i][1] == 't')
            {
                check_next_arg(argc, &i);
                global_ping.time_to_live = atoull(argv[i]);
            }
            else if (argv[i][1] == 's')
            {
                check_next_arg(argc, &i);
                global_ping.packet_size = atoull(argv[i]);
            }
            else if (argv[i][1] == 'c')
            {
                check_next_arg(argc, &i);
                global_ping.packet_count = atoi(argv[i]);
            }
            else if (argv[i][1] == 'i')
            {
                check_next_arg(argc, &i);
                global_ping.interval = atoull(argv[i]);
            }
            else
                exit(print_usage());
        }
        else if (global_ping.host)
            exit(print_usage());
        else
            global_ping.host = (char *)argv[i];
    }
    if (!global_ping.host)
        exit(print_usage());
}