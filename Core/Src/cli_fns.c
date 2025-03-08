#include "cli_fns.h"


cmd_t cmd_tbl[NUM_COMMANDS] = {
    {
        .cmd = "help",
        .func = help_func
    },
    {
        .cmd = "echo",
        .func = echo_func
    }
};


static cli_status_t help_func(int argc, char **argv)
{
    cli_status_t rslt = CLI_OK;

    /* Code executed when 'help' is entered */

    return rslt;
}

static cli_status_t echo_func(int argc, char **argv)
{
    cli_status_t rslt = CLI_OK;

    /* Code executed when 'echo' is entered */

    return rslt;
}