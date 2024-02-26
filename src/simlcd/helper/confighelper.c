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

int getsubtype(char const *a)
{
    int r = SIMLCD_TEXTWIDGET_STATICTEXT;

    r = strcicmp("gear", a);
    if (r == 0)
    {
        return SIMLCD_TEXTWIDGET_GEAR;
    }

    r = strcicmp("rpm", a);
    if (r == 0)
    {
        return SIMLCD_TEXTWIDGET_RPMS;
    }

    r = strcicmp("pos", a);
    if (r == 0)
    {
        return SIMLCD_TEXTWIDGET_POSITION;
    }

    r = strcicmp("numcars", a);
    if (r == 0)
    {
        return SIMLCD_TEXTWIDGET_NUMCARS;
    }

    r = strcicmp("lap", a);
    if (r == 0)
    {
        return SIMLCD_TEXTWIDGET_LAP;
    }

    r = strcicmp("lastlap", a);
    if (r == 0)
    {
        return SIMLCD_TEXTWIDGET_LASTLAP;
    }

    r = strcicmp("bestlap", a);
    if (r == 0)
    {
        return SIMLCD_TEXTWIDGET_BESTLAP;
    }

    r = strcicmp("brakebias", a);
    if (r == 0)
    {
        return SIMLCD_TEXTWIDGET_BRAKEBIAS;
    }

    r = strcicmp("fuelremaining", a);
    if (r == 0)
    {
        return SIMLCD_TEXTWIDGET_FUELREMAINING;
    }

    r = strcicmp("laps", a);
    if (r == 0)
    {
        return SIMLCD_TEXTWIDGET_LAPS;
    }

    return SIMLCD_TEXTWIDGET_STATICTEXT;
}



int configcheck(const char* config_file_str, int* fonts, int* widgets)
{
    config_t cfg;
    config_init(&cfg);
    if (!config_read_file(&cfg, config_file_str))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
    }
    config_setting_t* config_fonts = NULL;
    config_setting_t* config_widgets = NULL;
    config_fonts = config_lookup(&cfg, "fonts_array");
    *fonts = config_setting_length(config_fonts);
    config_widgets = config_lookup(&cfg, "widgets_array");
    *widgets = config_setting_length(config_widgets);
    config_destroy(&cfg);
    return 0;
    //return cfg;
}

int loadconfig(const char* config_file_str, Parameters* p, FontInfo* fi, SimlcdUIWidget* simlcdwidgets, const char* fontpath)
{
    config_t cfg;
    config_init(&cfg);
    if (!config_read_file(&cfg, config_file_str))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
    }
    else
    {
        slogi("Parsing config file");



        config_setting_t* config_font = NULL;
        config_setting_t* config_fonts = NULL;
        config_fonts = config_lookup(&cfg, "fonts_array");
        int fontslen = config_setting_length(config_fonts);
        slogd("will attempt to read %i fonts", fontslen);
        p->fonts_length = fontslen;


        const char* temp;
        for (int j = 0; j < fontslen; j++)
        {
            config_font = config_setting_get_elem(config_fonts, j);

            int found = config_setting_lookup_string(config_font, "name", &temp);
            found = config_setting_lookup_int(config_font, "size", &fi[j].size);
            // TODO: check if these are already full paths or just don't allow full paths and check multiple paths
            // better yet, find a library
            slogi("fount font %s", temp);
            size_t strzie = strlen(fontpath) + strlen(temp) + 1 + 1;
            char* temp2 = malloc(strzie);
            snprintf(temp2, strzie, "%s/%s", fontpath, temp);
            fi[j].name = strdup(temp2);
            free(temp2);
        }

        config_setting_t* config_widget = NULL;
        config_setting_t* config_widgets = NULL;
        config_widgets = config_lookup(&cfg, "widgets_array");
        int widgetslen = config_setting_length(config_widgets);
        slogd("will attempt to read %i widgets", widgetslen);
        p->widgets_length = widgetslen;

        for (int j = 0; j < widgetslen; j++)
        {
            config_widget = config_setting_get_elem(config_widgets, j);

            int found = config_setting_lookup_string(config_widget, "name", &temp);
            found = config_setting_lookup_string(config_widget, "type", &temp);
            simlcdwidgets[j].uiwidgettype = SIMLCD_UIWIDGET_TEXT;

            found = config_setting_lookup_string(config_widget, "subtype", &temp);
            simlcdwidgets[j].uiwidgetsubtype = getsubtype(temp);
            if (simlcdwidgets[j].uiwidgetsubtype == SIMLCD_TEXTWIDGET_STATICTEXT)
            {
                found = config_setting_lookup_string(config_widget, "text", &temp);
                simlcdwidgets[j].text = strdup(temp);
            }

            found = config_setting_lookup_int(config_widget, "fontid", &simlcdwidgets[j].fontid);
            found = config_setting_lookup_int(config_widget, "xpos", &simlcdwidgets[j].xpos);
            found = config_setting_lookup_int(config_widget, "ypos", &simlcdwidgets[j].ypos);
            found = config_setting_lookup_int(config_widget, "r", &simlcdwidgets[j].red);
            found = config_setting_lookup_int(config_widget, "g", &simlcdwidgets[j].green);
            found = config_setting_lookup_int(config_widget, "b", &simlcdwidgets[j].blue);
        }

    }


    config_destroy(&cfg);

    return 0;
}

