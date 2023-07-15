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
            ms->sim_name = SIMULATOR_SIMAPI_TEST;
        }
        else
        {
            slogi("%s does not appear to be a supported simulator.", game);
            return SIMLCD_ERROR_INVALID_SIM;
        }
    return SIMLCD_ERROR_NONE;
}

int configcheck(const char* config_file_str, int* fonts)
{
    config_t cfg;
    config_init(&cfg);
    if (!config_read_file(&cfg, config_file_str))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
    }
    config_setting_t* config_fonts = NULL;
    config_fonts = config_lookup(&cfg, "fonts_array");
    *fonts = config_setting_length(config_fonts);
    config_destroy(&cfg);
    return 0;
    //return cfg;
}

int loadconfig(const char* config_file_str, Parameters* p, FontInfo* fi)
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

        config_setting_t* config_fonts = NULL;
        config_fonts = config_lookup(&cfg, "fonts_array");
        int clen = config_setting_length(config_fonts);
        slogd("will attempt to read %i fonts", clen);
        p->fonts_length = clen;

        size_t a;
        config_setting_t* config_font = NULL;
        //for (int j = 0; j < clen; j++)
        //{

        //    const char* temp = config_setting_get_string_elem(config_fonts, j);
        //    a += sizeof(temp) + 1;
        //}
        //p->fonts = malloc(a);

        const char* temp;
        for (int j = 0; j < clen; j++)
        {
            config_font = config_setting_get_elem(config_fonts, j);

            int found = config_setting_lookup_string(config_font, "name", &temp);
            found = config_setting_lookup_int(config_font, "size", &fi[j].size);
            slogi("fount font %s", temp);
            fi[j].name = strdup(temp);
        }
        slogi("font 0 is %s", fi[0].name);
    }


    config_destroy(&cfg);

    return 0;
}

