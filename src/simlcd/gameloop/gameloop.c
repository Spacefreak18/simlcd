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

gfx_color_t pal16[18];

#define DEFAULT_UPDATE_RATE      60.0

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


int showstats(SimData* simdata)
{
    printf("\r");
    for (int i=0; i<4; i++)
    {
        if (i==0)
        {
            fputc('s', stdout);
            fputc('p', stdout);
            fputc('e', stdout);
            fputc('e', stdout);
            fputc('d', stdout);
            fputc(':', stdout);
            fputc(' ', stdout);

            int speed = simdata->velocity;
            int digits = 0;
            if (speed > 0)
            {
                while (speed > 0)
                {
                    int mod = speed % 10;
                    speed = speed / 10;
                    digits++;
                }
                speed = simdata->velocity;
                int s[digits];
                int digit = 0;
                while (speed > 0)
                {
                    int mod = speed % 10;
                    s[digit] = mod;
                    speed = speed / 10;
                    digit++;
                }
                speed = simdata->velocity;
                digit = digits;
                while (digit > 0)
                {
                    fputc(s[digit-1]+'0', stdout);
                    digit--;
                }
            }
            else
            {
                fputc('0', stdout);
            }
            fputc(' ', stdout);
        }
        if (i==1)
        {
            fputc('r', stdout);
            fputc('p', stdout);
            fputc('m', stdout);
            fputc('s', stdout);
            fputc(':', stdout);
            fputc(' ', stdout);

            int rpms = simdata->rpms;
            int digits = 0;
            if (rpms > 0)
            {
                while (rpms > 0)
                {
                    int mod = rpms % 10;
                    rpms = rpms / 10;
                    digits++;
                }
                rpms = simdata->rpms;
                int s[digits];
                int digit = 0;
                while (rpms > 0)
                {
                    int mod = rpms % 10;
                    s[digit] = mod;
                    rpms = rpms / 10;
                    digit++;
                }
                rpms = simdata->rpms;
                digit = digits;
                while (digit > 0)
                {
                    fputc(s[digit-1]+'0', stdout);
                    digit--;
                }
            }
            else
            {
                fputc('0', stdout);
            }
            fputc(' ', stdout);
        }
        if (i==2)
        {
            fputc('g', stdout);
            fputc('e', stdout);
            fputc('a', stdout);
            fputc('r', stdout);
            fputc(':', stdout);
            fputc(' ', stdout);
            fputc(simdata->gear+'0', stdout);
            fputc(' ', stdout);
        }
        if (i==3)
        {
            fputc('a', stdout);
            fputc('l', stdout);
            fputc('t', stdout);
            fputc(':', stdout);
            fputc(' ', stdout);

            int alt = simdata->altitude;
            int digits = 0;
            if (alt > 0)
            {
                while (alt > 0)
                {
                    int mod = alt % 10;
                    alt = alt / 10;
                    digits++;
                }
                alt = simdata->altitude;
                int s[digits];
                int digit = 0;
                while (alt > 0)
                {
                    int mod = alt % 10;
                    s[digit] = mod;
                    alt = alt / 10;
                    digit++;
                }
                alt = simdata->altitude;
                digit = digits;
                while (digit > 0)
                {
                    fputc(s[digit-1]+'0', stdout);
                    digit--;
                }
            }
            else
            {
                fputc('0', stdout);
            }
            fputc(' ', stdout);
        }
    }
    fflush(stdout);
}

void sighandler(int signum, siginfo_t* info, void* ptr)
{
    go = false;
    //gfx_clear(pixels, pixels_len);
    //gfx_swapbuffers();
    //gfx_close();
}

struct sigaction act;

int looper()
{
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = sighandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTSTP, &act, NULL);

    SimData* simdata = malloc(sizeof(SimData));
    SimMap* simmap = malloc(sizeof(SimMap));
    int error = siminit(simdata, simmap, 1);

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
    FT_Face face_font32;
    FT_Library ft_font32;
    init_ft("./font.ttf", &face_font32, &ft_font32, 32, &err);

    FT_Face face_font48;
    FT_Library ft_font48;
    init_ft("./font.ttf", &face_font48, &ft_font48, 48, &err);

    FT_Face face_font96;
    FT_Library ft_font96;
    init_ft("./font.ttf", &face_font96, &ft_font96, 96, &err);

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


        simdatamap(simdata, simmap, 1);
        //time(&now);
        //tm = *localtime(&now);


        gfx_clear(pixels, pixels_len);

        //strftime(date, sizeof(date), "%Y.%m.%d", &tm);


        char gear[2];
        gear[0] = simdata->gear + 48;
        if (simdata->gear == 0) {
            gear[0] = 'N';
        }
        if (simdata->gear < 0) {
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


        {
            draw_string_on_fb(face_font32, ft_font32, pixels, 10, 25, pal16[16], "Lap: ");
            draw_string_on_fb(face_font32, ft_font32, pixels, 80, 25, pal16[16], lap);
            draw_string_on_fb(face_font32, ft_font32, pixels, 136, 25, pal16[16], " / ");
            draw_string_on_fb(face_font32, ft_font32, pixels, 160, 25, pal16[16], numlaps);

            draw_string_on_fb(face_font32, ft_font32, pixels, 600, 25, pal16[16], "Pos: ");
            draw_string_on_fb(face_font32, ft_font32, pixels, 670, 25, pal16[16], pos);
            draw_string_on_fb(face_font32, ft_font32, pixels, 726, 25, pal16[16], " / ");
            draw_string_on_fb(face_font32, ft_font32, pixels, 750, 25, pal16[16], numcars);
        }

        {
            draw_string_on_fb(face_font32, ft_font32, pixels, 30, 210, pal16[17], "Fuel: ");
        }

        {
            draw_string_on_fb(face_font48, ft_font48, pixels, xres/2 - (xres*.06125), yres/2 - (yres*.30), pal16[16], rpm);
        }

        {
            draw_string_on_fb(face_font96, ft_font96, pixels, xres/2 - (xres*.025), yres/2 - (yres*0), pal16[16], gear);
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

    done_ft(ft_font32);

    done_ft(ft_font48);

    done_ft(ft_font96);

    gfx_clear(pixels, pixels_len);
    gfx_swapbuffers();

    tcsetattr(0, TCSANOW, &canonicalmode);
    gfx_close();

    free(simdata);
    free(simmap);
}

