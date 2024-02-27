#include "parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libconfig.h>

#include <argtable2.h>
#include <regex.h>


int freeparams(Parameters* p)
{
    if (p->config_path != NULL)
    {
        free(p->config_path);
    }
    if (p->font_path != NULL)
    {
        free(p->font_path);
    }
    return 0;
}

ConfigError getParameters(int argc, char** argv, Parameters* p)
{

    ConfigError exitcode = E_SOMETHING_BAD;

    // set return structure defaults
    p->program_action      = 0;
    p->verbosity_count     = 0;

    // setup argument handling structures
    const char* progname = "simlcd";

    struct arg_lit* arg_verbosity   = arg_litn("v","verbose", 0, 2, "increase logging verbosity");

    struct arg_rex* cmd1             = arg_rex1(NULL, NULL, "play", NULL, REG_ICASE, NULL);
    struct arg_str* arg_cpath        = arg_strn("c", "configpath", "<path of config>", 0, 1, NULL);
    struct arg_str* arg_fpath        = arg_strn("f", "fontpath", "<path of fonts>", 0, 1, NULL);
    struct arg_str* arg_sim          = arg_strn("s", "sim", "<gamename>", 0, 1, NULL);
    struct arg_lit* help             = arg_litn(NULL,"help", 0, 1, "print this help and exit");
    struct arg_lit* vers             = arg_litn(NULL,"version", 0, 1, "print version information and exit");
    struct arg_end* end              = arg_end(20);
    void* argtable[]                 = {cmd1,arg_sim,arg_verbosity,arg_cpath,arg_fpath,help,vers,end};
    int nerrors;



    if (arg_nullcheck(argtable) != 0)
    {
        printf("%s: insufficient memory\n",progname);
        goto cleanup;
    }

    nerrors = arg_parse(argc,argv,argtable);

    if (nerrors==0)
    {
        p->program_action = A_PLAY;
        p->sim_string = strdup(arg_sim->sval[0]);
        p->verbosity_count = arg_verbosity->count;
        if (arg_cpath->sval[0] != NULL)
        {
            p->config_path = strdup(arg_cpath->sval[0]);
        }
        if (arg_fpath->sval[0] != NULL)
        {
            p->font_path = strdup(arg_fpath->sval[0]);
        }
        exitcode = E_SUCCESS_AND_DO;
    }

    // interpret some special cases before we go through trouble of reading the config file
    if (help->count > 0)
    {
        printf("Usage: %s\n", progname);
        arg_print_syntax(stdout,argtable,"\n");
        printf("\nReport bugs on the github github.com/spacefreak18/simlcd.\n");
        exitcode = E_SUCCESS_AND_EXIT;
        goto cleanup;
    }

    if (vers->count > 0)
    {
        printf("%s Simulator Dashboard\n",progname);
        printf("June 2023, Paul Dino Jones\n");
        exitcode = E_SUCCESS_AND_EXIT;
        goto cleanup;
    }

cleanup:
    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
    return exitcode;

}
