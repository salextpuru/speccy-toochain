// ZX-Spectrum SCREEN$ editor
// (c) 2004 Catalin Mihaila <mihaila_ac@yahoo.com>

#include "main.h"

// ink selected from palette:
int new_ink            = 0;
// paper selected from palette:
int new_paper          = 7;

int new_bright         = 0;
int new_flash          = 0;

int xofattrs           = SCREENWIDTH-256;
int yofattrs           = 230;



int show_current_selected_attribure (int curattrs_x, int curattrs_y, int sel_ink, int sel_paper)
{
    if(debug) { printf("Show current attrs\n"); }
    
    printstring("INK:", curattrs_x, curattrs_y, 0, bgcol);
    if(debug) { printf("INK is: %d\n", sel_ink); }
    clear_rect(curattrs_x+108, curattrs_y, 16, 16, sel_ink);
    draw_grid(curattrs_x+108, curattrs_y, 1, 1, gridcol);

    printstring("PAPER:", curattrs_x, curattrs_y+20, 0, bgcol);
    if(debug) { printf("PAPER is: %d\n", sel_paper); }
    clear_rect(curattrs_x+108, curattrs_y+20, 16, 16, sel_paper);
    draw_grid(curattrs_x+108, curattrs_y+20, 1, 1, gridcol);

    if((sel_ink > 7) && (sel_paper > 7)) { new_bright = 1; } else { new_bright = 0; }

    printstring("BRIGHT:", curattrs_x, curattrs_y+40, 0, bgcol);
    if(debug) { printf("BRIGHT is: %d\n", new_bright); }
    if(new_bright) { clear_rect(curattrs_x+108, curattrs_y+40, 16, 16, 15); }
    else { clear_rect(curattrs_x+108, curattrs_y+40, 16, 16, 0); }
    draw_grid(curattrs_x+108, curattrs_y+40, 1, 1, gridcol);

    printstring("FLASH:", curattrs_x, curattrs_y+60, 0, bgcol);
    if(debug) { printf("FLASH is: %d\n", new_flash); }
    if(new_flash) { clear_rect(curattrs_x+108, curattrs_y+60, 16, 16, 15); }
    else { clear_rect(curattrs_x+108, curattrs_y+60, 16, 16, 0); }
    draw_grid(curattrs_x+108, curattrs_y+60, 1, 1, gridcol);

    printstring("GRID:", curattrs_x, curattrs_y+80, 0, bgcol);
    if(grid_is_visible) { clear_rect(curattrs_x+108, curattrs_y+80, 16, 16, 15); }
    else { clear_rect(curattrs_x+108, curattrs_y+80, 16, 16, 0); }
    draw_grid(curattrs_x+108, curattrs_y+80, 1, 1, gridcol);

    return 0;
}

void get_new_attrs_of_cell(int changeline, int changecol)
{
    int attribute=0;

    attribute=get_me_attrnr_of_cell (line, col);
    new_ink = get_ink_by_attr(attrs[attribute]);
    new_paper = get_paper_by_attr(attrs[attribute]);
    new_flash = get_flash_by_attr(attrs[attribute]);
}

int get_me_attrnr_of_cell (int oneline, int onecol)
{
    int l,c;
    int attrib=0;

    for(l=0;l<(oneline+1);l++) { for(c=0;c<(onecol+1);c++) { attrib=(c+(l*32)); } }

    return attrib;
}
