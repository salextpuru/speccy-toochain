// ZX-Spectrum SCREEN$ editor
// (c) 2004 Catalin Mihaila <mihaila_ac@yahoo.com>

#include "main.h"

void move_right (void)
{
    if(debug) { printf("Right arrow pressed\n");
                printf("Current cell: line - %d / col %d\n",line,col);
              }
    if(col < 31) { col++; }
    finish_move();
}

void move_left (void)
{
    if(debug) { printf("Left arrow pressed\n");
                printf("Current cell: line - %d / col %d\n",line,col);
              }
    if(col > 0) { col--; }
    finish_move();
}

void move_up (void)
{
    if(debug) { printf("UP arrow pressed\n");
                printf("Current cell: line - %d / col %d\n",line,col);
              }
    if(line > 0) { line--; }
    finish_move();
}

void move_down (void)
{
    if(debug) { printf("Down arrow pressed\n");
                printf("Current cell: line - %d / col %d\n",line,col);
              }
    if(line < 23) { line++; }
    finish_move();
}

void finish_move (void)
{
    int cur_attrib;
    cur_attrib=get_me_attrnr_of_cell (line, col);
    new_ink=get_ink_by_attr(attrs[cur_attrib]);
    new_paper=get_paper_by_attr(attrs[cur_attrib]);
    redraw_all();
}
