#include "traceroute.h"

// Parses command line arguments and updates the traceroute options accordingly.
// @param argc the number of arguments
// @param argv an array of strings containing the arguments
// @param options a pointer to the traceroute_options struct to be updated
// @return void
void parse_options(int argc, char **argv, traceroute_options *options)
{
    int i;

    // Parse command line arguments
    for (i = 1; i < argc; i++)
    {
        char *arg = argv[i];

        if (strings_equal(arg, "--help") || strings_equal(arg, "-h"))
        {
            print_help_text();
            exit(EXIT_SUCCESS);
        }
        else if (strings_equal(arg, "-d"))
        {
            options->debug = true;
        }
        else if (strings_equal(arg, "-I"))
        {
            options->packet_type = ICMP_ECHO;
        }
        else if (strings_equal(arg, "-f"))
        {
            if (i == argc - 1)
            {
                handle_error("missing argument to -f");
            }
            i++;
            options->first_ttl = atoull(argv[i]);
            if (options->first_ttl == 0)
            {
                handle_error("first hop out of range");
            }
        }
        else if (strings_equal(arg, "-m"))
        {
            if (i == argc - 1)
            {
                handle_error("missing argument to -m");
            }
            i++;
            options->max_ttl = atoull(argv[i]);
            if (options->max_ttl == 0)
            {
                handle_error("max_ttl should not be 0!");
            }
            if (options->max_ttl > 255)
            {
                handle_error("max hops cannot be more than 255");
            }
        }
        else if (strings_equal(arg, "-q"))
        {
            if (i == argc - 1)
            {
                handle_error("missing argument to -q");
            }
            i++;
            options->probes_per_ttl = atoull(argv[i]);
            if (options->probes_per_ttl <= 0 || options->probes_per_ttl > 10)
            {
                handle_error("use a valid probes number");
            }
        }
        else if (arg[0] == '-')
        {
            handle_error("unknown option");
        }
        else
        {
            if (options->target_host != NULL)
            {
                handle_error("too many arguments");
            }
            options->target_host = arg;
        }
    }

    if (options->target_host == NULL)
    {
        handle_error("missing target host");
    }

    if (options->first_ttl > options->max_ttl)
    {
        handle_error("first hop already out of range");
    }
}
