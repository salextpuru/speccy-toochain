// ZX-Spectrum SCREEN$ editor
// (c) 2004 Catalin Mihaila <mihaila_ac@yahoo.com>

#include "main.h"

int xofcell            = SCREENWIDTH-128;
int yofcell            = 214;
int attr               = 0;
int square_colour      = 18;

int cell_mem_addr      = 0;
char cell_pixels[8][8];


// draw current selected cell:
int draw_cell (int cell_line, int cell_col)
{
    int attrib = 0;
    int l,c,i;
    int k=0;
    int s=0;
    int d=1;
    int lineofpixels = 0;
    int lopbinar[8];
    int *plopbin;

    if(debug) { printf("Draw cell: line - %d / col - %d\n", cell_line, cell_col); attr_debug = 1;}

// find what attribute have requested cell:
    attrib=get_me_attrnr_of_cell (line, col);

// set paper of cell and draw:
    if(get_flash_by_attr(attrs[attrib]) && !flash_state) {
	clear_rect(xofcell, yofcell, 128, 128, get_ink_by_attr(attrs[attrib]));
    } else {
	clear_rect(xofcell, yofcell, 128, 128, get_paper_by_attr(attrs[attrib]));
    }

// set also ink...
    get_ink_by_attr(attrs[attrib]);

// draw 8 lines of pixels for requested cell:

    if(debug) { printf("Draw pixels in cell\n"); }

    for(l=0;l<8;l++)
      {
          if(cell_line < 8)
              {
                  lineofpixels = zxscr[attrib+(256*l)];
                  cell_mem_addr = attrib;
              }

          if((cell_line > 7) && (cell_line < 16))
              {
                  lineofpixels = zxscr[attrib+(256*l)+1792];
                  cell_mem_addr = (attrib+1792);
              }

          if(cell_line > 15)
              {
                  lineofpixels = zxscr[attrib+(256*l)+3584];
                  cell_mem_addr = (attrib+3584);
              }

          plopbin = lopbinar;

          for(c=0;c<8;c++)
              {
                  if(lineofpixels&d)
                      {
                          *plopbin = 1;
                      } else {
                          *plopbin = 0;
                      }
                  d=d*2;
                  plopbin++;
              }
          d = 1;

          for(i=0;i<8;i++) { cell_pixels[l][i] = lopbinar[i]; }

          for(i=7;i>-1;i--)
              {
                  if(lopbinar[i]) {
if(get_flash_by_attr(attrs[attrib]) && !flash_state) {
		  clear_rect(xofcell+k, yofcell+s, 16, 16, get_paper_by_attr(attrs[attrib]));
} else {
		  clear_rect(xofcell+k, yofcell+s, 16, 16, get_ink_by_attr(attrs[attrib]));
}
                                  }
                  k+=16;
              }
            k=0;
            s+=16;
      }

    attr = attrib;

// draw grid of cell
    if(grid_is_visible) { draw_grid(xofcell,yofcell,8,8,gridcol); }

// draw square on big screen
    draw_grid (cell_col*16, cell_line*16, 1, 1, square_colour);

    attr_debug = 0;

    return 0;
}
