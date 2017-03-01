// ZX-Spectrum SCREEN$ editor
// (c) 2004 Catalin Mihaila <mihaila_ac@yahoo.com>

#include "main.h"

int loading_x              = 0;

int loading_y              = SCREENHEIGHT-192;
int fix_loading_y          = SCREENHEIGHT-192;

int line_inc               = 1;
int zone_inc               = 0;

Uint32 pulse               = 0;
Uint32 text_speed =0;
int finished_load          = 0;
int attributes_done        = 0;

int le_octet_attr          = 0;
int le_octet_video         = 0;
int fix_octet_video        = 0;
int msgptr                 = 0;
int textlimit              = 0;
int text_to_show           = 1;
int nofm                   = 9;


// is draw ?
unsigned char square_draw[768];



void loading_efect (void)
{
    int pixel,mask;
    int pixel_color;
    int lineeight,oneline;
    int sqd;

	Uint32 counting_ticks;
    Uint32 px;


    if(finished_load) { put_attributes(); return; }

	counting_ticks = SDL_GetTicks();
	if(counting_ticks < pulse+150) return;
	pulse = counting_ticks;

// draw one line of 32:
    for(oneline=0;oneline<32;oneline++) {
	pixel = zxscr[le_octet_video];


	mask = 0x80;
// draw 8 pixels on line:
	for(lineeight=0;lineeight<8;lineeight++) {
// draw only black & white:
	    pixel_color = (pixel & mask) ? 0 : 192;

	    px=SDL_MapRGB(scrbuf->format, pixel_color, pixel_color, pixel_color);

	    mask = ((mask >> 1) & 0x7F);
	    putpixel(scrbuf,loading_x,loading_y,px);
	    SDL_UpdateRect(scrbuf,loading_x,loading_y,1,1);

	    loading_x++;
	}

	le_octet_video++;
    }

    loading_x=0;
    loading_y+=8;

// take care of y
    if((loading_y >= (SCREENHEIGHT-128)) && (zone_inc == 0))
	{
// move y to up
	    loading_y=fix_loading_y+line_inc;
	    line_inc++;
	    if(line_inc > 8) { zone_inc++; loading_y=fix_loading_y+64; line_inc=1;}
	}

    if((loading_y >= (SCREENHEIGHT-64)) && (zone_inc == 1))
	{
// move y to up
	    loading_y=fix_loading_y+64+line_inc;
	    line_inc++;
	    if(line_inc > 8) { zone_inc++; loading_y=fix_loading_y+128;  line_inc=1;}
	}

    if((loading_y >= (SCREENHEIGHT)) && (zone_inc == 2))
	{
// move y to up
	    loading_y=fix_loading_y+128+line_inc;
	    line_inc++;
	    if(line_inc > 8) {
		loading_x = 0;
		loading_y = SCREENHEIGHT-192;
		fix_loading_y = SCREENHEIGHT-192;
		line_inc = 1;
		zone_inc = 0;
		finished_load = 1;
		attributes_done = 0;
		le_octet_attr = 0;
		le_octet_video = 0;
		for(sqd=0;sqd<768;sqd++) square_draw[sqd] = 0;
	    }
	}
}

void put_attributes (void)
{
    int ink,paper;
    int onecell,oneoct;
    int pixel,mask;
    int pixel_color;
    int line_a=0;

	Uint32 counting_ticks;
    Uint32 px;

    if(attributes_done) return;

	counting_ticks = SDL_GetTicks();
	if(counting_ticks < pulse+8) return;
	pulse = counting_ticks;

    le_octet_video+=le_octet_attr;
    if(le_octet_attr >255) le_octet_video+=1792;
    if(le_octet_attr >511) le_octet_video+=1792;



    ink=get_ink_by_attr(attrs[le_octet_attr]);
    paper=get_paper_by_attr(attrs[le_octet_attr]);

    for(onecell=0;onecell<8;onecell++)
	{

    mask = 0x80;
    pixel = zxscr[le_octet_video+line_a*256];

    for(oneoct=0;oneoct<8;oneoct++)
	{
	    pixel_color = (pixel & mask) ? ink : paper;

	    px=SDL_MapRGB(scrbuf->format, colour_palette[0+3*pixel_color],
					  colour_palette[1+3*pixel_color],
					  colour_palette[2+3*pixel_color]);

	    mask = ((mask >> 1) & 0x7F);
	    putpixel(scrbuf,loading_x,loading_y,px);
	    loading_x++;
	}
	loading_x-=8;
	SDL_UpdateRect(scrbuf,loading_x,loading_y,8,1);
	loading_y++;
	line_a++;
	}

    loading_y-=8;


    loading_x+=8;

    if(loading_x > 255 ) { loading_x=0; loading_y+=8; }

    le_octet_attr++;
    le_octet_video = 0;

    if(le_octet_attr > 767) { attributes_done = 1; loading_x=0; loading_y=fix_loading_y; attributes_done=1;}
    else { square_draw[le_octet_attr-1] = 1; }
    return;
}

void text_scroll_efect (void)
{
	Uint32 counting_ticks;
	int left_limit;
	char *mesaj=NULL;
	int linie;
	Uint32 linie_col;

	switch(text_to_show)
	{
		case 1:
			mesaj="Welcome to ZXscredit!                        ";
		break;

		case 2:
			mesaj="This program is distributed under the terms of the GNU GPL license version 2                        ";
		break;

		case 3:
			mesaj="Usage: zxscredit [filename.scr]                        ";
		break;

		case 4:
			mesaj="Press F1 for about info.                        ";
		break;

		case 5:
			mesaj="Press F2 for starting with a new file.                        ";
		break;

		case 6:
			mesaj="Press F3 for save current screen and tape file.                        ";
		break;

		case 7:
			mesaj="Press F5 or G to toggle grid.                         ";
		break;

		case 8:
			mesaj="Press F10 or ESC to exit.                        ";
		break;

		case 9:
			mesaj="Please report any bug to mihaila_ac@yahoo.com                        ";
		break;
	}


	counting_ticks = SDL_GetTicks();
	if(counting_ticks < text_speed+150) return;
	text_speed = counting_ticks;


	clear_rect(272,SCREENHEIGHT-32,392,32,15);

	linie_col=SDL_MapRGB(scrbuf->format,255,0,0);

	for(linie=272;linie<(272+392);linie++)
	   {
		   putpixel(scrbuf, linie, SCREENHEIGHT-32, linie_col);
		   putpixel(scrbuf, linie, SCREENHEIGHT-1, linie_col);
		   if(linie<304)
				{
				   putpixel(scrbuf, 272, (SCREENHEIGHT-304+linie), linie_col);
				   putpixel(scrbuf, 272+392, (SCREENHEIGHT-304+linie), linie_col);
				}
	}


	for(left_limit=textlimit;left_limit<msgptr;left_limit++)
	printchar(scrbuf->pixels, mesaj[left_limit], 660-(16*msgptr)+(16*left_limit), SCREENHEIGHT-24, 0, 15, 0);

	msgptr++;

	if(msgptr>strlen(mesaj)){ msgptr=0; textlimit=0; text_to_show++; }
	if(msgptr>24) textlimit++;
	if(text_to_show > nofm) text_to_show=1;

	SDL_UpdateRect(scrbuf,272,SCREENHEIGHT-32,392,32);
}
