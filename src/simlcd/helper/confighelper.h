#ifndef _CONFIGHELPER_H
#define _CONFIGHELPER_H

#include <stdbool.h>
#include <stdint.h>

#include <libconfig.h>

#include "parameters.h"

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

typedef enum
{
    SIMLCD_UIWIDGET_TEXT       = 0,
    SIMLCD_UIWIDGET_BOX        = 1,
}
SimlcdWidgetType;

typedef enum
{
    SIMLCD_TEXTWIDGET_STATICTEXT      = 0,
    SIMLCD_TEXTWIDGET_RPMS            = 1,
    SIMLCD_TEXTWIDGET_GEAR            = 2,
    SIMLCD_TEXTWIDGET_LAP             = 3,
    SIMLCD_TEXTWIDGET_LAPS            = 4,
    SIMLCD_TEXTWIDGET_POSITION        = 5,
    SIMLCD_TEXTWIDGET_NUMCARS         = 6,
    SIMLCD_TEXTWIDGET_BESTLAP         = 7,
    SIMLCD_TEXTWIDGET_LASTLAP         = 8,
    SIMLCD_TEXTWIDGET_BRAKEBIAS       = 9,
    SIMLCD_TEXTWIDGET_FUELREMAINING   = 10,
}
SimlcdWidgetSubType;


typedef struct
{
    ProgramAction program_action;
    Simulator sim_name;
}
SimlcdSettings;

typedef struct
{
    int size;
    char* name;
}
FontInfo;

typedef struct
{
    SimlcdWidgetType uiwidgettype;
    SimlcdWidgetSubType uiwidgetsubtype;
    int fontid;
    int xpos;
    int ypos;
    int red;
    int green;
    int blue;
    char* text;
    //char* name;
}
SimlcdUIWidget;


int strtogame(const char* game, SimlcdSettings* ms);

int configcheck(const char* config_file_str, int* fonts, int* widgets);

int loadconfig(const char* config_file_str, Parameters* p, FontInfo* fi, SimlcdUIWidget* simlcdwidgets, const char* fontpath);

#endif
