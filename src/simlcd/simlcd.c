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

    ConfigError ppe = getParameters(argc, argv, p);
    if (ppe == E_SUCCESS_AND_EXIT)
    {
        goto cleanup_final;
    }

    char* home_dir_str = gethome();
    char* configdir = create_user_dir(".config", "simlcd");
    char* cachedir = create_user_dir(".local/share", "simlcd");
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
    char* font_path = get_dir_with_default(p->font_path, "/usr/share/fonts/TTF");

    int fonts = 0;
    int widgets = 0;
    configcheck(config_file_str, &fonts, &widgets);
    FontInfo* fi = malloc(sizeof(FontInfo) * fonts);
    SimlcdUIWidget* simlcdwidgets = malloc(sizeof(SimlcdUIWidget) * widgets);
    loadconfig(config_file_str, p, fi, simlcdwidgets, font_path);

    free(config_file_str);
    xdgWipeHandle(&xdg);

    slogi("starting visuals");

    looper(fi, fonts, simlcdwidgets, widgets, p);

    for (int j = 0; j < fonts; j++)
    {
        free(fi[j].name);
    }
    free(fi);
    for (int j = 0; j < fonts; j++)
    {
        if (simlcdwidgets[j].uiwidgetsubtype == SIMLCD_TEXTWIDGET_STATICTEXT)
        {
            free(simlcdwidgets[j].text);
        }
    }
    free(simlcdwidgets);


configcleanup:

cleanup_final:
    freeparams(p);

    free(configdir);
    free(cachedir);
    
    exit(0);
}


