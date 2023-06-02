#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include <libconfig.h>
#include "gameloop/gameloop.h"
#include "helper/parameters.h"
#include "helper/dirhelper.h"
#include "helper/confighelper.h"
#include "slog/slog.h"

int create_dir(char* dir)
{
    struct stat st = {0};
    if (stat(dir, &st) == -1)
    {
        mkdir(dir, 0700);
    }
}

char* create_user_dir(char* dirtype)
{
    char* home_dir_str = gethome();
    char* config_dir_str = ( char* ) malloc(1 + strlen(home_dir_str) + strlen(dirtype) + strlen("simlcd/"));
    strcpy(config_dir_str, home_dir_str);
    strcat(config_dir_str, dirtype);
    strcat(config_dir_str, "simlcd");

    create_dir(config_dir_str);
    free(config_dir_str);
}


int main(int argc, char** argv)
{
    Parameters* p = malloc(sizeof(Parameters));
    SimlcdSettings* ms = malloc(sizeof(SimlcdSettings));;

    ConfigError ppe = getParameters(argc, argv, p);
    if (ppe == E_SUCCESS_AND_EXIT)
    {
        goto cleanup_final;
    }
    ms->program_action = p->program_action;

    char* home_dir_str = gethome();
    create_user_dir("/.config/");
    create_user_dir("/.cache/");
    char* config_file_str = ( char* ) malloc(1 + strlen(home_dir_str) + strlen("/.config/") + strlen("simlcd/simlcd.config"));
    char* cache_dir_str = ( char* ) malloc(1 + strlen(home_dir_str) + strlen("/.cache/simlcd/"));
    strcpy(config_file_str, home_dir_str);
    strcat(config_file_str, "/.config/");
    strcpy(cache_dir_str, home_dir_str);
    strcat(cache_dir_str, "/.cache/simlcd/");
    strcat(config_file_str, "simlcd/simlcd.config");

    slog_config_t slgCfg;
    slog_config_get(&slgCfg);
    slgCfg.eColorFormat = SLOG_COLORING_TAG;
    slgCfg.eDateControl = SLOG_TIME_ONLY;
    strcpy(slgCfg.sFileName, "simlcd.log");
    strcpy(slgCfg.sFilePath, cache_dir_str);
    slgCfg.nTraceTid = 0;
    slgCfg.nToScreen = 1;
    slgCfg.nUseHeap = 0;
    slgCfg.nToFile = 1;
    slgCfg.nFlush = 0;
    slgCfg.nFlags = SLOG_FLAGS_ALL;
    slog_config_set(&slgCfg);
    if (p->verbosity_count < 2)
    {
        slog_disable(SLOG_TRACE);
    }
    if (p->verbosity_count < 1)
    {
        slog_disable(SLOG_DEBUG);
    }

    slogi("starting visuals");

    looper();
    //slogi("Loading configuration file: %s", config_file_str);
    //config_t cfg;
    //config_init(&cfg);
    //config_setting_t* config_devices = NULL;

    //if (!config_read_file(&cfg, config_file_str))
    //{
    //    fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
    //}
    //else
    //{
    //    slogi("Openend simlcd configuration file");
    //    config_devices = config_lookup(&cfg, "devices");
    //}
    //free(config_file_str);
    //free(cache_dir_str);

    //if (p->program_action == A_CONFIG_TACH)
    //{
    //}
    //else
    //{


    //    int error = 0;

    //    int configureddevices = config_setting_length(config_devices);
    //    int numdevices = 0;
    //    DeviceSettings ds[configureddevices];
    //    slogi("found %i devices in configuration", configureddevices);
    //    int i = 0;
    //    while (i<configureddevices)
    //    {
    //        error = SIMLCD_ERROR_NONE;
    //        DeviceSettings settings;
    //        ds[i] = settings;
    //        config_setting_t* config_device = config_setting_get_elem(config_devices, i);
    //        const char* device_type;
    //        const char* device_subtype;
    //        const char* device_config_file;
    //        config_setting_lookup_string(config_device, "device", &device_type);
    //        config_setting_lookup_string(config_device, "type", &device_subtype);
    //        config_setting_lookup_string(config_device, "config", &device_config_file);

    //        if (error == SIMLCD_ERROR_NONE)
    //        {
    //            error = devsetup(device_type, device_subtype, device_config_file, ms, &ds[i], config_device);
    //        }
    //        if (error == SIMLCD_ERROR_NONE)
    //        {
    //            numdevices++;
    //        }

    //        i++;

    //    }

    //}



configcleanup:
    //config_destroy(&cfg);

cleanup_final:
    free(ms);
    free(p);
    exit(0);
}


