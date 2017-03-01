// ZX-Spectrum SCREEN$ editor
// (c) 2004 Catalin Mihaila <mihaila_ac@yahoo.com>

#include "main.h"

int screen_is_modif = 0;
int ask             = 0;
int want_new_file   = 0;


void new_empty_file (void)
{
    int filenew = 0;

	screen_name = NULL;

    for(filenew=0;filenew<6144;filenew++)
        {
            zxscr[filenew] = 0;
        }

	for(filenew=0;filenew<768;filenew++)
        {
            attrs[filenew] = 56;
        }
    screen_is_modif = 0;
    ask = 0;
    want_new_file = 0;
    line = 0;
    col = 0;

	get_new_attrs_of_cell(line, col);

    flash_pause = 0;

    small_scr = 1;
    big_scr = 1;
	le_octet_video = 0;
	loading_x  = 0;
	loading_y = SCREENHEIGHT - 192;
	line_inc = 1;
	zone_inc = 0;
}
