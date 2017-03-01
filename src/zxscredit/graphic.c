// ZX-Spectrum SCREEN$ editor
// (c) 2004 Catalin Mihaila <mihaila_ac@yahoo.com>

#include "main.h"

SDL_Surface *scrbuf;

// init SDL:
int init_graphic_lib (void)
{
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 )
        {
            if(debug) { printf("FAILED\n"); }
            return 1;
        } else {
            if(debug) { printf("OK\n"); }
            return 0;
        }
}


// open window:
int init_graphic_window (void)
{
    char windowtitle[256];

    sprintf(windowtitle,CAPTION,MAJV,MINV);

    scrbuf = SDL_SetVideoMode( SCREENWIDTH, SCREENHEIGHT, 16, SDL_SWSURFACE|SDL_ANYFORMAT);//|SDL_FULLSCREEN );
    if( scrbuf == NULL )
        {
            if(debug) { printf("FAILED\n"); }
            return(1);
        }

    SDL_WM_SetCaption( windowtitle, "" );

    if(debug) { printf("OK\n"); }
    return 0;
}

// done... close SDL:
int close_graphic_lib (void)
{
    if(debug) { printf("Quit SDL...\n"); }
    SDL_Quit();
    return 0;
}

int update_screen (void)
{
    if(debug) { printf("Update all screen\n"); }
    SDL_UpdateRect(scrbuf,0,0,0,0);
    return 0;
}
