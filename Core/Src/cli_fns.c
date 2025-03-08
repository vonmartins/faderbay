#include "cli_fns.h"
#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "flash.h"
#include "config.h"


cmd_t cmd_tbl[NUM_COMMANDS] = {
    {
        .cmd = "help",
        .func = help_func
    },
    {
        .cmd = "set-midi-channel",
        .func = set_ch_func
    },
    {
        .cmd = "set-fader-cc",
        .func = set_cc_func
    },
    {
        .cmd = "save-current-preset",
        .func = save_current_preset
    },
    {
        .cmd = "save-preset",
        .func = save_preset
    },
    {
        .cmd = "set-default-config",
        .func = set_default_config
    },
    {
        .cmd = "erase-preset",
        .func = erase_preset
    }
};


static cli_status_t help_func(int argc, char **argv)
{
    cli.println("Available commands:");
    cli.println("------------------------------------------------------------");
    cli.println("help                                   - Show this help menu");
    cli.println("set-midi-channel <fader> <channel>     - Set MIDI channel for a fader (1-16)");
    cli.println("set-fader-cc <fader> <cc>              - Set MIDI CC for a fader (0-127)");
    cli.println("save-current-preset                    - Save the current configuration to the active preset");
    cli.println("save-preset <preset>                   - Save the current configuration to a specific preset");
    cli.println("------------------------------------------------------------");
    cli.println("Note: Fader numbers range from 1 to NUM_FADERS");
    return CLI_OK;
}

static cli_status_t set_cc_func(int argc, char **argv)
{
    if (argc != 3)
    {
        cli.println("Usage: set-fader-cc <fader> <channel>");
        return CLI_E_INVALID_ARGS;
    }

    uint8_t fader = atoi(argv[1]);
    uint8_t cc = atoi(argv[2]);

    if (fader > NUM_FADERS || fader < 1)
    {
        cli.println(" Error: Invalid fader");
        return CLI_E_INVALID_ARGS;
    }

    if (cc > 127)
    {
        cli.println(" Error: Invalid cc value (0-127)");
        return CLI_E_INVALID_ARGS;
    }

    device.current_config.midi_ccs[fader-1] = cc;
    if(!Write_Configs(&device.current_config))
    {
        /* TO DO */
        cli.println("Error: Memory not written");
    }
    
    char buff[40];  
    sprintf(buff, "Fader %d CC set to %d\n", fader, cc);  
    cli.println((char *)buff);
    return CLI_OK;
}

static cli_status_t set_ch_func(int argc, char **argv)
{
    if (argc != 3)
    {
        cli.println("Usage: set-midi-channel <fader> <channel>");
        return CLI_E_INVALID_ARGS;
    }

    uint8_t fader = atoi(argv[1]);
    uint8_t channel = atoi(argv[2]);

    if (fader > NUM_FADERS)
    {
        cli.println(" Error: Invalid fader");
        return CLI_E_INVALID_ARGS;
    }

    if (channel > 16)
    {
        cli.println(" Error: Invalid midi channel (1-16)");
        return CLI_E_INVALID_ARGS;
    }

    device.current_config.midi_channels[fader-1] = channel;
    if(!Write_Configs(&device.current_config))
    {
        /* TO DO */
        cli.println("Error: Memory not written");
    }
    
    char buff[40];  
    sprintf(buff, "Fader %d MIDI channel set to %d\n", fader, channel);  
    cli.println((char *)buff);
    return CLI_OK;
}


static cli_status_t save_current_preset(int argc, char **argv)
{
    if (argc != 1) 
    {
        cli.println("Usage: save-current-preset");
        return CLI_E_INVALID_ARGS;
    }

    uint8_t preset_index = device.current_preset;
    
    memcpy(device.presets[preset_index].preset_channels, device.current_config.midi_channels, sizeof(device.current_config.midi_channels));
    memcpy(device.presets[preset_index].preset_ccs, device.current_config.midi_ccs, sizeof(device.current_config.midi_ccs));

    if(!Write_Presets(&device.presets[preset_index]))
    {
        /* TO DO */
        cli.println("Error: Memory not written");
    }

    char buff[40];  
    sprintf(buff, "Preset %d saved successfully\n", preset_index);  /* See if \n is needed */
    cli.println(buff);

    return CLI_OK;
}

static cli_status_t save_preset(int argc, char **argv)
{
    if (argc != 2) 
    {
        cli.println("Usage: save-preset <preset>");
        return CLI_E_INVALID_ARGS;
    }

    uint8_t preset_index = atoi(argv[1]);

    if (preset_index >= NUM_PRESETS) {
        cli.println("Error: Invalid preset number");
        return CLI_E_INVALID_ARGS;
    }
    
    memcpy(device.presets[preset_index].preset_channels, device.current_config.midi_channels, sizeof(device.current_config.midi_channels));
    memcpy(device.presets[preset_index].preset_ccs, device.current_config.midi_ccs, sizeof(device.current_config.midi_ccs));

    if(!Write_Presets(&device.presets[preset_index]))
    {
        /* TO DO */
        cli.println("Error: Memory not written");
    }
    device.current_preset = preset_index;

    char buff[40];  
    sprintf(buff, "Preset %d saved successfully\n", preset_index);  /* See if \n is needed */
    cli.println(buff);

    return CLI_OK;
}

static cli_status_t set_default_config(int argc, char **argv)
{
    if(argc != 1)
    {
        cli.println("Usage: set-default-config");
        return CLI_E_INVALID_ARGS; 
    }
    Set_Config_Default(&device.current_config);
    return CLI_OK;
}

static cli_status_t erase_preset(int argc, char **argv)
{
    if(argc != 2)
    {
        cli.println("Usage: erase-preset <preset>");
        return CLI_E_INVALID_ARGS; 
    }
    uint8_t preset_index = atoi(argv[1]);
    Erase_Preset(&device, preset_index);

    char buff[40];  
    sprintf(buff, "Preset %d erased successfully\n", preset_index);
    cli.println(buff);
    return CLI_OK;
}


