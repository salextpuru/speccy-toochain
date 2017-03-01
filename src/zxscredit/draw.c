// ZX-Spectrum SCREEN$ editor
// (c) 2004 Catalin Mihaila <mihaila_ac@yahoo.com>

#include "main.h"

int bgcol              = 16;
int gridcol            = 17;
int grid_is_visible    = 1;

int small_scr          = 1; // show small screen ?
int big_scr            = 1; // show big screen ?

// were to draw palette ?
int xofpalette         = SCREENWIDTH-256;
int yofpalette         = 195;

unsigned int colour_palette[] = {
    000,000,000,
    000,000,192,
    192,000,000,
    192,000,192,
    000,192,000,
    000,192,192,
    192,192,000,
    192,192,192,
    000,000,000,
    000,000,255,
    255,000,000,
    255,000,255,
    000,255,000,
    000,255,255,
    255,255,000,
    255,255,255,
    110,110,110, // background
    150,200,250, // grid
    000,130,000  // square_colour
};

// draw one rectangle from x,y with width and height, fill with colour from palette:
int clear_rect (int start_x, int start_y, int rect_width, int rect_height, int rect_fill_colour)
{
    Uint32 rect_pixel;
    int rect_pix;
    int rect_piy;

    if(debug) { printf("Draw rectangle X: %d - Y: %d - W: %d - H: %d - C: %d\n",
     start_x, start_y, rect_width, rect_height, rect_fill_colour); }

    rect_pixel=SDL_MapRGB(scrbuf->format,
                          colour_palette[0+3*rect_fill_colour],
                          colour_palette[1+3*rect_fill_colour],
                          colour_palette[2+3*rect_fill_colour]
                         );

    for(rect_pix=start_x;rect_pix<(start_x+rect_width);rect_pix++)
        {
            for(rect_piy=start_y;rect_piy<(start_y+rect_height);rect_piy++)
                {
                    putpixel(scrbuf,rect_pix,rect_piy,rect_pixel);
                }
        }
    return 0;
}

// Put one pixel on a surface at x,y:
int putpixel (SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
        case 1:
            *p = pixel;
        break;

        case 2:
            *(Uint16 *)p = pixel;
        break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
        break;

        case 4:
            *(Uint32 *)p = pixel;
        break;
    }

    return 0;
}

int draw_screen (void)
{
    int octetnr;
    int octetline;
    int zoneline;
    int zonenr;

    if(debug) { printf("Draw preview and magnified screens\n"); }

    for(zonenr=0;zonenr<3;zonenr++) {
    for(zoneline=0;zoneline<8;zoneline++) {
    for(octetline=0;octetline<8;octetline++) {
    for(octetnr=(zoneline*32);octetnr<((zoneline*32)+32);octetnr++) {
        draw_screen_pixels(zxscr[octetnr+(256*octetline)+(zonenr*2048)],
                           attrs[octetnr+(256*zonenr)],
                           (((8*octetnr)-(256*zoneline)))+SCREENWIDTH-256,
                           (octetline+(8*zoneline)+(zonenr*64)),
			   octetnr+(256*zonenr));
    }
    }
    }
    }

// draw grid for magnified screen:
    if(grid_is_visible) { draw_grid (0,0,24,32,gridcol); }

    return 0;
}

int draw_screen_pixels (int octet_value, int attr_nr, int pix, int piy, int attr_pos)
{
    int pixels,values;
    int ink,paper;
    int coordx,multiplic;

    Uint32 px;

    values = 128;

    ink = get_ink_by_attr(attr_nr);
    paper = get_paper_by_attr(attr_nr);
    get_flash_by_attr(attr_nr);

    for(pixels=0;pixels<8;pixels++)
	{
	    if(octet_value & values)
                {
// pixel have ink colour:
		if(!flash_state && new_flash)
		    {
                    px=SDL_MapRGB(scrbuf->format, colour_palette[0+3*paper],
                				  colour_palette[1+3*paper],
						  colour_palette[2+3*paper]);
		    } else {
                    px=SDL_MapRGB(scrbuf->format, colour_palette[0+3*ink],
                				  colour_palette[1+3*ink],
						  colour_palette[2+3*ink]);
		    }
                } else {
// pixel have paper colour:
		if(!flash_state && new_flash)
		    {
                    px=SDL_MapRGB(scrbuf->format, colour_palette[0+3*ink],
                				  colour_palette[1+3*ink],
						  colour_palette[2+3*ink]);
		    } else {
                    px=SDL_MapRGB(scrbuf->format, colour_palette[0+3*paper],
                				  colour_palette[1+3*paper],
						  colour_palette[2+3*paper]);
		    }
                }
// put pixel on preview:
    if(small_scr) putpixel(scrbuf,pix,piy,px);
    if(square_draw[attr_pos]) putpixel(scrbuf,pix-544,piy+SCREENHEIGHT-192,px);

// put same pixel by 4 times on magnified screen:
    if(big_scr)
	{
            coordx=2*(pix-SCREENWIDTH+256);
            for(multiplic=0;multiplic<2;multiplic++)
                {
                    putpixel(scrbuf,coordx,2*piy,px);
                    putpixel(scrbuf,coordx,2*piy+1,px);
                    coordx++;
                }
	}
            pix++;
	    values /= 2;
        }
    return 0;
}

// extract and return ink value from attribute:
int get_ink_by_attr (int attrnr)
{
    int ink,bright;

    ink=(attrnr & 0x07);
    bright=(attrnr & 0x40);


    if(bright) { ink=ink+8; }

//    new_ink = ink;

//    if(debug && attr_debug) { printf("INK by attr is: %d for attr %d\n", new_ink, attrnr); }
    return ink;
}

// extract and return paper value from attribute:
int get_paper_by_attr (int attrnr)
{
    int paper,bright;

    paper=((attrnr >> 3) & 0x07);
    bright=(attrnr & 0x40);

    if(bright) { paper=paper+8; }

//    new_paper = paper;

//    if(debug && attr_debug) { printf("PAPER by attr is: %d for attr %d\n", new_paper, attrnr); }
    return paper;
}

int get_flash_by_attr (int attrnr)
{

    new_flash=(attrnr & 0x80);

    if(debug && attr_debug) { printf("FLASH by attr is: %d for attr %d\n", new_flash, attrnr); }
    return new_flash;
}

// draw grid from x,y for (lines x cols), lines with colour from palette:
int draw_grid (int gr_x, int gr_y, int gr_nrlines, int gr_nrcols, int gr_colour)
{
    Uint32 grpixcol;
    int incx    = 0;
    int incy    = 0;
    int nrpix;          // pixels per line or col
    int nrcols;
    int nrlines;

    if(debug) { printf("Draw grid: X - %d / Y - %d / L - %d / C - %d / c - %d\n",
                                       gr_x, gr_y, gr_nrlines, gr_nrcols, gr_colour); }

    grpixcol=SDL_MapRGB(scrbuf->format,
                        colour_palette[  3*gr_colour],
                        colour_palette[1+3*gr_colour],
                        colour_palette[2+3*gr_colour]
                       );

    for(nrlines=0;nrlines<gr_nrlines;nrlines++) {
    for(nrcols=0;nrcols<gr_nrcols;nrcols++) {
    for(nrpix=0;nrpix<16;nrpix++) {
    putpixel(scrbuf, gr_x+nrpix+incx, gr_y+incy, grpixcol);
    putpixel(scrbuf, gr_x+incx, gr_y+nrpix+incy, grpixcol);
    }
    incx+=16;
    }
    incx = 0;
    incy+=16;
    }

// close grid:
    for(nrcols=0;nrcols<gr_nrcols;nrcols++) {
    for(nrpix=0;nrpix<16;nrpix++) {
    putpixel(scrbuf, gr_x+nrpix+incx, gr_y+incy-1, grpixcol);
    }
    incx+=16;
    }

    incy = 0;

    for(nrlines=0;nrlines<gr_nrlines;nrlines++) {
    for(nrpix=0;nrpix<16;nrpix++) {
    putpixel(scrbuf, gr_x+incx-1, gr_y+nrpix+incy, grpixcol);
    }
    incy+=16;
    }

    return 0;
}

// generate 16x16 square for each of 16 colours from palette:
int draw_colour_palette (int palx, int paly)
{
    Uint32 px;
    int sq_width;               // width of 16x16 colour square
    int numcolors;
    int palettex = palx;
    int palettey;               // height of 16x16 colour square


    if(debug) { printf("Draw colour palette: X - %d / Y - %d\n", palx, paly); }

    for(numcolors=0;numcolors<16;numcolors++)
        {
            px=SDL_MapRGB(scrbuf->format,
                          colour_palette[0+3*numcolors],
                          colour_palette[1+3*numcolors],
                          colour_palette[2+3*numcolors]
                         );

            for(sq_width=0;sq_width<16;sq_width++) {
            for(palettey=0;palettey<16;palettey++) {
                putpixel(scrbuf,palettex,paly+palettey,px);
            }
            palettex++;
            }
        }

// draw pallete grid:
    draw_grid(palx,paly,1,16,gridcol);

    return 0;
}

void redraw_all (void)
{
    if(debug) { printf("Redraw screen\n"); }

// reactivate flash
    flash_pause=0;


// show current selected ink / paper / bright / flash
    show_current_selected_attribure(xofattrs, yofattrs, new_ink, new_paper);

    draw_screen();
    draw_cell (line, col);

    update_screen();
}

int make_attr ( int what_ink, int what_paper)
{
    if(debug) { printf("Make ATTR: INK: %d / PAPER: %d / BRIGHT: %d / FLASH: %d\n",
                                      what_ink, what_paper, new_bright, new_flash); }
    if(what_ink > 7 ) { what_ink = (what_ink - 8); }
    return ((8*what_paper)+what_ink);
}
