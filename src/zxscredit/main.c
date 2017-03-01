// ZX-Spectrum SCREEN$ editor
// (c) 2004 Catalin Mihaila <mihaila_ac@yahoo.com>


#include "main.h"

#ifdef USE_DEBUG
int debug = 1;
int attr_debug = 0;
#else
int debug = 0;
int attr_debug = 0;
#endif

unsigned char zxscr[6144];
unsigned char attrs[768];

int line               = 0;
int col                = 0;

char *screen_name = NULL;

int main (int argc, char *argv[])
{
    FILE *fisier;
    int mainloop = 1;

if(debug) { printf("%s: START\n",argv[0]); }


    screen_name = argv[1];
    fisier = fopen(screen_name,"rb");
    if(fisier)
	{
	    if(debug) { printf("Reading SCREEN$ from file %s\n",screen_name); }
            fread(zxscr,6144,1,fisier);
            fread(attrs,768,1,fisier);
            fclose(fisier);
        } else {
	    if(debug) { printf("Creating a new SCREEN$\n"); }
	    new_empty_file();
        }

    if(debug) { printf("Init SDL: "); }
    if(init_graphic_lib()) { return 1; }

    if(debug) { printf("Open main window: "); }
    if(init_graphic_window()) { return 1; }

// set background:
    if(debug) { printf("Clear screen\n"); }
    clear_rect(0,0,SCREENWIDTH,SCREENHEIGHT,bgcol);
    clear_rect(0,SCREENHEIGHT-192,256,192,7);

// put control buttons:
    draw_icons();

// draw screens:
    draw_screen();

// draw colour palette:
    draw_colour_palette(xofpalette,yofpalette);

// draw current cell:
    draw_cell(line, col);

    new_ink = get_ink_by_attr(attrs[0]);
    new_paper = get_paper_by_attr(attrs[0]);

// show current selected ink / paper
    show_current_selected_attribure(xofattrs, yofattrs, new_ink, new_paper);

	pulse = SDL_GetTicks();

// refresh screen:
    update_screen();

    while(mainloop)
	{
	    mainloop=handle_events();
		text_scroll_efect();
	    do_flash();
	    loading_efect();
	}

    close_graphic_lib();

    return 0;
}
