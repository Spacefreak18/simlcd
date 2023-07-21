#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <poll.h>
#include <termios.h>
#include <signal.h>
#include <math.h>

#include "gameloop.h"
#include "../helper/parameters.h"
#include "../helper/confighelper.h"
#include "../simulatorapi/simapi/simapi/simdata.h"
#include "../simulatorapi/simapi/simapi/simmapper.h"
#include "../slog/slog.h"

#include "../fbgfx/src/fbgfx.h"
#include "../fbgfx/src/fbbuf.h"
#include "../fbgfx/src/render16.h"
#include "../fbgfx/src/font16.h"
#include "../fbgfx/src/render.h"
#include "../fbgfx/src/ttfont.h"

bool go = true;
size_t pixels_len, rowbytes;
unsigned char* pixels;
unsigned xres, yres;

struct fttinfo
{
    FT_Face ft_face;
    FT_Library ft_library;
};

gfx_color_t pal16[18];

#define DEFAULT_UPDATE_RATE      60.0
#define SIM_CHECK_RATE           1

/******************************************************************************/

#define MIN(a, b) ({ \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })
#define MAX(a, b) ({ \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })

/******************************************************************************/


void sighandler(int signum, siginfo_t* info, void* ptr)
{
    go = false;
    //gfx_clear(pixels, pixels_len);
    //gfx_swapbuffers();
    //gfx_close();
}

struct sigaction act;




int clilooper(FontInfo* fi, int fonts, SimlcdUIWidget* simlcdwidgets, int widgets, SimData* simdata, SimMap* simmap, int sim)
{
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = sighandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTSTP, &act, NULL);



    if (gfx_open(&xres, &yres, &rowbytes))
    {
        return 1;
    }

    if (gfx_setbpp(16, &rowbytes))
    {
        return 1;
    }


    draw_init(16);

    pixels = gfx_buffer(&pixels_len);
    if (!pixels)
    {
        return 1;
    }

    gfx_clear(pixels, pixels_len);

    pal_init();

    struct termios newsettings, canonicalmode;
    tcgetattr(0, &canonicalmode);
    newsettings = canonicalmode;
    newsettings.c_lflag &= (~ICANON & ~ECHO);
    newsettings.c_cc[VMIN] = 1;
    newsettings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &newsettings);
    char ch;
    struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };

    char* err = NULL;
    struct fttinfo* ft = malloc(sizeof(struct fttinfo) * fonts);
    for (int j = 0; j < fonts; j++)
    {
        init_ft(fi[j].name, &ft[j].ft_face, &ft[j].ft_library, fi[j].size, &err);
    }


    while (go) {
    	//time_t now;
    	struct tm tm;
    	int ox, oy, cx, cy;
    	int r;
    	int i;
    	//char date[40];

    	ox = xres / 2;
    	oy = yres / 2;

    	r = MIN(xres, yres) * 2 / 3 / 2;


        simdatamap(simdata, simmap, sim);
        //time(&now);
        //tm = *localtime(&now);


        gfx_clear(pixels, pixels_len);

        //strftime(date, sizeof(date), "%Y.%m.%d", &tm);


        char gear[2];
        gear[0] = simdata->gear + 47;
        if (simdata->gear == 1) {
            gear[0] = 'N';
        }
        if (simdata->gear < 1) {
            gear[0] = 'R';
        }
        gear[1] = '\0';
        UTF32 gear2[2] = {gear[0], 0};

        char numlaps[4];
        char numcars[4];
        char pos[4];
        char lap[4];
        char rpm[6];

        sprintf(rpm, "%i", simdata->rpms);
        sprintf(numlaps, "%03d", simdata->numlaps);
        sprintf(numcars, "%03d", simdata->numcars);
        sprintf(lap, "%03d", simdata->lap);
        sprintf(pos, "%03d", simdata->position);

        numlaps[3] = '\0';
        numcars[3] = '\0';
        pos[3] = '\0';
        lap[3] = '\0';
        rpm[5] = '\0';

        for (int j = 0; j < widgets; j++)
        {

            char* tempstr;
            bool draw = false;
            if (simlcdwidgets[j].uiwidgetsubtype == SIMLCD_TEXTWIDGET_GEAR)
            {
                tempstr = gear;
                draw = true;
            }
            if (simlcdwidgets[j].uiwidgetsubtype == SIMLCD_TEXTWIDGET_RPMS)
            {
                tempstr = rpm;
                draw = true;
            }
            if (simlcdwidgets[j].uiwidgetsubtype == SIMLCD_TEXTWIDGET_POSITION)
            {
                tempstr = pos;
                draw = true;
            }
            if (simlcdwidgets[j].uiwidgetsubtype == SIMLCD_TEXTWIDGET_NUMCARS)
            {
                tempstr = numcars;
                draw = true;
            }
            if (simlcdwidgets[j].uiwidgetsubtype == SIMLCD_TEXTWIDGET_LAP)
            {
                tempstr = lap;
                draw = true;
            }
            if (simlcdwidgets[j].uiwidgetsubtype == SIMLCD_TEXTWIDGET_LAPS)
            {
                tempstr = numlaps;
                draw = true;
            }
            if (simlcdwidgets[j].uiwidgetsubtype == SIMLCD_TEXTWIDGET_STATICTEXT)
            {
                tempstr = simlcdwidgets[j].text;
                draw = true;
            }
            if (draw == true)
            {
                draw_string_on_fb(
                        ft[simlcdwidgets[j].fontid].ft_face, ft[simlcdwidgets[j].fontid].ft_library, pixels, 
                        simlcdwidgets[j].xpos, simlcdwidgets[j].ypos,
                        gfx_rgb(simlcdwidgets[j].red,simlcdwidgets[j].green,simlcdwidgets[j].blue), tempstr);
            }
        }


        gfx_swapbuffers();

    	if( poll(&mypoll, 1, 1000.0/30) )
    	{
    	    scanf("%c", &ch);
    	    if(ch == 'q')
    	    {
    	        go = false;
    	    }
    	}
    }

    
    for (int j = 0; j < fonts; j++)
    {
        done_ft(ft[j].ft_library);
    }


    gfx_clear(pixels, pixels_len);
    gfx_swapbuffers();

    tcsetattr(0, TCSANOW, &canonicalmode);
    gfx_close();

}

int looper(FontInfo* fi, int fonts, SimlcdUIWidget* simlcdwidgets, int widgets, Parameters* p)
{
    SimData* simdata = malloc(sizeof(SimData));
    SimMap* simmap = malloc(sizeof(SimMap));

    struct termios newsettings, canonicalmode;
    tcgetattr(0, &canonicalmode);
    newsettings = canonicalmode;
    newsettings.c_lflag &= (~ICANON & ~ECHO);
    newsettings.c_cc[VMIN] = 1;
    newsettings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &newsettings);
    char ch;
    struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };

    fprintf(stdout, "Searching for sim data... Press q to quit...\n");

    p->simon = false;
    double update_rate = SIM_CHECK_RATE;
    int go = true;

    while (go == true)
    {


        if (p->simon == false)
        {
            getSim(simdata, simmap, &p->simon, &p->sim);
        }

        if (p->simon == true)
        {
            clilooper(fi, fonts, simlcdwidgets, widgets, simdata, simmap, p->sim);
        }
        if (p->simon == true)
        {
            p->simon = false;
            fprintf(stdout, "Searching for sim data... Press q again to quit...\n");
            sleep(2);
        }

        if( poll(&mypoll, 1, 1000.0/update_rate) )
        {
            scanf("%c", &ch);
            if(ch == 'q')
            {
                go = false;
            }
        }
    }

    fprintf(stdout, "\n");
    fflush(stdout);
    tcsetattr(0, TCSANOW, &canonicalmode);

    free(simdata);
    free(simmap);

    return 0;
}
