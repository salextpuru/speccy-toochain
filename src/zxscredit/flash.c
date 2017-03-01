// ZX-Spectrum SCREEN$ editor
// (c) 2004 Catalin Mihaila <mihaila_ac@yahoo.com>

#include "main.h"

int seconds                = 0;
unsigned char flash_flag   = 0;
unsigned char flash_state  = 0;
int flash_pause            = 0;
int five_secs              = 0;

time_t timp;
struct tm *flashing;

void do_flash (void)
{
    int new_seconds;

    if(flash_pause) { return; }

    timp=time(NULL);

    flashing=localtime(&timp);

    new_seconds = flashing->tm_sec;

    if(!new_seconds) { flash_flag=0; }

    if(new_seconds > seconds) { flash_flag=1-flash_flag; }

    seconds = new_seconds;

    set_new_flash();
}

void set_new_flash (void)
{
    int sqd;

    if(flash_flag == flash_state) { return; }
    flash_state=1-flash_state;

// count 5 seconds and restart loading efect:

    if(attributes_done && finished_load)
	{
	    five_secs++;
	    if(five_secs > 5)
		{
		    clear_rect(0,SCREENHEIGHT-192,256,192,7);
		    finished_load = 0;
		    five_secs=0;
		    for(sqd=0;sqd<768;sqd++) {
			square_draw[sqd] = 0;
		    }
		}
	}

    redraw_all();
}
