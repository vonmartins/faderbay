#ifndef _CLI_FNS_H
#define _CLI_FNS_H

#include "C:\Development\Repos\faderbay\Apps\cli\cli.h"


#define NUM_COMMANDS    5

extern cmd_t cmd_tbl[NUM_COMMANDS];



static cli_status_t help_func(int argc, char **argv);
static cli_status_t set_cc_func(int argc, char **argv);
static cli_status_t set_ch_func(int argc, char **argv);
static cli_status_t save_current_preset(int argc, char **argv);
static cli_status_t save_preset(int argc, char **argv);




#endif

