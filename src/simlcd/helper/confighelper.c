#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>


#include "confighelper.h"

#include "../slog/slog.h"


int strcicmp(char const *a, char const *b)
{
    for (;; a++, b++) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}


int strtogame(const char* game, SimlcdSettings* ms)
{
    slogd("Checking for %s in list of supported simulators.", game);
    if (strcicmp(game, "ac") == 0)
    {
        slogd("Setting simulator to Assetto Corsa");
        ms->sim_name = SIMULATOR_ASSETTO_CORSA;
    }
    else if (strcicmp(game, "rf2") == 0)
    {
        slogd("Setting simulator to RFactor 2");
        ms->sim_name = SIMULATOR_RFACTOR2;
    }
    else
        if (strcicmp(game, "test") == 0)
        {
            slogd("Setting simulator to Test Data");
            ms->sim_name = SIMULATOR_SIMLCD_TEST;
        }
        else
        {
            slogi("%s does not appear to be a supported simulator.", game);
            return SIMLCD_ERROR_INVALID_SIM;
        }
    return SIMLCD_ERROR_NONE;
}

int strtodevsubtype(const char* device_subtype, DeviceSettings* ds, int simdev)
{
    ds->is_valid = false;
    ds->dev_subtype = SIMDEVTYPE_UNKNOWN;

    switch (simdev) {
        case SIMDEV_USB:
            if (strcicmp(device_subtype, "Tachometer") == 0)
            {
                ds->dev_subtype = SIMDEVTYPE_TACHOMETER;
                break;
            }
        case SIMDEV_SERIAL:
            if (strcicmp(device_subtype, "ShiftLights") == 0)
            {
                ds->dev_subtype = SIMDEVTYPE_SHIFTLIGHTS;
                break;
            }
            if (strcicmp(device_subtype, "SimWind") == 0)
            {
                ds->dev_subtype = SIMDEVTYPE_SIMWIND;
                break;
            }
        case SIMDEV_SOUND:
            if (strcicmp(device_subtype, "Engine") == 0)
            {
                ds->dev_subtype = SIMDEVTYPE_ENGINESOUND;
                break;
            }
            if (strcicmp(device_subtype, "Gear") == 0)
            {
                ds->dev_subtype = SIMDEVTYPE_GEARSOUND;
                break;
            }
        default:
            ds->is_valid = false;
            slogw("%s does not appear to be a valid device sub type, but attempting to continue with other devices", device_subtype);
            return SIMLCD_ERROR_INVALID_DEV;
    }
    ds->is_valid = true;
    return SIMLCD_ERROR_NONE;
}

int strtodev(const char* device_type, const char* device_subtype, DeviceSettings* ds)
{
    ds->is_valid = false;
    if (strcicmp(device_type, "USB") == 0)
    {
        ds->dev_type = SIMDEV_USB;
        strtodevsubtype(device_subtype, ds, SIMDEV_USB);
    }
    else
        if (strcicmp(device_type, "Sound") == 0)
        {
            ds->dev_type = SIMDEV_SOUND;
            strtodevsubtype(device_subtype, ds, SIMDEV_SOUND);
        }
        else
            if (strcicmp(device_type, "Serial") == 0)
            {
                ds->dev_type = SIMDEV_SERIAL;
                strtodevsubtype(device_subtype, ds, SIMDEV_SERIAL);
            }
            else
            {
                ds->is_valid = false;
                slogi("%s does not appear to be a valid device type, but attempting to continue with other devices", device_type);
                return SIMLCD_ERROR_INVALID_DEV;
            }
    ds->is_valid = true;
    return SIMLCD_ERROR_NONE;
}

int loadconfig(const char* config_file, DeviceSettings* ds)
{
    return 0;
}

int devsetup(const char* device_type, const char* device_subtype, const char* config_file, SimlcdSettings* ms, DeviceSettings* ds, config_setting_t* device_settings)
{
    int error = SIMLCD_ERROR_NONE;
    slogi("Called device setup with %s %s %s", device_type, device_subtype, config_file);
    ds->dev_type = SIMDEV_UNKNOWN;

    error = strtodev(device_type, device_subtype, ds);
    if (error != SIMLCD_ERROR_NONE)
    {
        return error;
    }

    if (ms->program_action == A_PLAY || ms->program_action == A_TEST)
    {
        error = loadconfig(config_file, ds);
    }
    if (error != SIMLCD_ERROR_NONE)
    {
        return error;
    }

    if (ds->dev_subtype == SIMDEVTYPE_TACHOMETER)
    {
        if (device_settings != NULL)
        {
            config_setting_lookup_int(device_settings, "granularity", &ds->tachsettings.granularity);
            if (ds->tachsettings.granularity < 0 || ds->tachsettings.granularity > 4 || ds->tachsettings.granularity == 3)
            {
                slogd("No or invalid valid set for tachometer granularity, setting to 1");
                ds->tachsettings.granularity = 1;
            }
            slogi("Tachometer granularity set to %i", ds->tachsettings.granularity);
        }
        ds->tachsettings.use_pulses = true;
        if (ms->program_action == A_PLAY || ms->program_action == A_TEST)
        {
            ds->tachsettings.use_pulses = false;
        }
    }
    if (ds->dev_subtype == SIMDEVTYPE_SIMWIND || ds->dev_subtype == SIMDEVTYPE_SHIFTLIGHTS)
    {
        if (device_settings != NULL)
        {
            const char* temp;
            config_setting_lookup_string(device_settings, "devpath", &temp);
            ds->serialdevsettings.portdev = strdup(temp);
        }
    }

    return error;
}

int settingsfree(DeviceSettings ds)
{

    if (ds.dev_subtype == SIMDEVTYPE_SIMWIND || ds.dev_subtype == SIMDEVTYPE_SHIFTLIGHTS)
    {
        if (ds.serialdevsettings.portdev != NULL)
        {
            free(ds.serialdevsettings.portdev);
        }
    }

    return 0;
}
