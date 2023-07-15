#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <libconfig.h>
#include <basedir_fs.h>

#include "gameloop/gameloop.h"
#include "helper/parameters.h"
#include "helper/dirhelper.h"
#include "helper/confighelper.h"
#include "slog/slog.h"


int main(int argc, char** argv)
{
    Parameters* p = malloc(sizeof(Parameters));
    //SimlcdSettings* ms = malloc(sizeof(SimlcdSettings));;

    ConfigError ppe = getParameters(argc, argv, p);
    if (ppe == E_SUCCESS_AND_EXIT)
    {
        goto cleanup_final;
    }
    //ms->program_action = p->program_action;

    char* home_dir_str = gethome();
    char* configdir = create_user_dir(".config", "simlcd");
    char* cachedir = create_user_dir(".local/share", "simlcd");
    ////char* config_file_str = ( char* ) malloc(1 + strlen(home_dir_str) + strlen("/.config/") + strlen("simlcd/simlcd.configchar* cache_dir_str = ( char* ) malloc(1 + strlen(home_dir_str) + strlen("/.cache/simlcd/"));
    //char* cache_dir_str = ( char* ) malloc(1 + strlen(home_dir_str) + strlen("/.cache/simlcd/"));
    ////strcat(config_file_str, "/.config/");
    //strcpy(cache_dir_str, home_dir_str);
    //strcat(cache_dir_str, "/.cache/simlcd/");
    ////strcat(config_file_str, "simlcd/simlcd.config");

    slog_config_t slgCfg;
    slog_config_get(&slgCfg);
    slgCfg.eColorFormat = SLOG_COLORING_TAG;
    slgCfg.eDateControl = SLOG_TIME_ONLY;
    strcpy(slgCfg.sFileName, "simlcd.log");
    strcpy(slgCfg.sFilePath, cachedir);
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

    xdgHandle xdg;
    if(!xdgInitHandle(&xdg))
    {
        slogf("Function xdgInitHandle() failed, is $HOME unset?");
    }
    char* config_file_str = get_config_file(p->config_path, &xdg);


    int fonts = 0;
    configcheck(config_file_str, &fonts);
    slogi("found %i fonts", fonts);
    FontInfo* fi = malloc(sizeof(FontInfo) * 2);
    loadconfig(config_file_str, p, fi);

    free(config_file_str);
    //free(cache_dir_str);
    xdgWipeHandle(&xdg);

    slogi("starting visuals");

    for (int j = 0; j < fonts; j++)
    {
        slogi("fount font %s", fi[j].name);
    }
    looper(fi, fonts, p);
    for (int j = 0; j < fonts; j++)
    {
        free(fi[j].name);
    }
    free(fi);


configcleanup:
    //config_destroy(&cfg);

cleanup_final:
    freeparams(p);
    //free(ms);
    //free(p);

    free(configdir);
    free(cachedir);
    
    exit(0);
}


