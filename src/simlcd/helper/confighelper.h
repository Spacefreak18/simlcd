#ifndef _CONFIGHELPER_H
#define _CONFIGHELPER_H

#include <stdbool.h>
#include <stdint.h>

#include <libconfig.h>

#include "parameters.h"

typedef enum
{
    SIMDEV_UNKNOWN    = 0,
    SIMDEV_USB        = 1,
    SIMDEV_SOUND      = 2,
    SIMDEV_SERIAL     = 3
}
DeviceType;

typedef enum
{
    SIMDEVTYPE_UNKNOWN           = 0,
    SIMDEVTYPE_TACHOMETER        = 1,
    SIMDEVTYPE_SHIFTLIGHTS       = 2,
    SIMDEVTYPE_SIMWIND           = 3,
    SIMDEVTYPE_ENGINESOUND       = 4,
    SIMDEVTYPE_GEARSOUND         = 5
}
DeviceSubType;

typedef enum
{
    SIMULATOR_SIMLCD_TEST    = 0,
    SIMULATOR_ASSETTO_CORSA     = 1,
    SIMULATOR_RFACTOR           = 2,
    SIMULATOR_RFACTOR2          = 3
}
Simulator;

typedef enum
{
    SIMULATOR_UPDATE_DEFAULT    = 0,
    SIMULATOR_UPDATE_RPMS       = 1,
    SIMULATOR_UPDATE_GEAR       = 2,
    SIMULATOR_UPDATE_PULSES     = 3,
    SIMULATOR_UPDATE_VELOCITY   = 4,
    SIMULATOR_UPDATE_ALTITUDE   = 5
}
SimulatorUpdate;

typedef enum
{
    SIMLCD_ERROR_NONE          = 0,
    SIMLCD_ERROR_UNKNOWN       = 1,
    SIMLCD_ERROR_INVALID_SIM   = 2,
    SIMLCD_ERROR_INVALID_DEV   = 3,
    SIMLCD_ERROR_NODATA        = 4,
    SIMLCD_ERROR_UNKNOWN_DEV   = 5
}
SimlcdError;

typedef struct
{
    ProgramAction program_action;
    Simulator sim_name;
}
SimlcdSettings;

typedef struct
{
    int size;
    bool use_pulses;
    int granularity;
    uint32_t* rpms_array;
    uint32_t* pulses_array;
}
TachometerSettings;

typedef struct
{
    char* portdev;
}
SerialDeviceSettings;

typedef struct
{
    bool is_valid;
    DeviceType dev_type;
    DeviceSubType dev_subtype;
    TachometerSettings tachsettings;
    SerialDeviceSettings serialdevsettings;
}
DeviceSettings;

int strtogame(const char* game, SimlcdSettings* ms);

int devsetup(const char* device_type, const char* device_subtype, const char* config_files, SimlcdSettings* ms, DeviceSettings* ds, config_setting_t* device_settings);

int settingsfree(DeviceSettings ds);

#endif
