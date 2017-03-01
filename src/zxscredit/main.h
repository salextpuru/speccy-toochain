// ZX-Spectrum SCREEN$ editor
// (c) 2004 Catalin Mihaila <mihaila_ac@yahoo.com>

#ifndef MAIN_H
#define MAIN_H

#include <SDL/SDL.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define MAJV 0
#define MINV 9
#define CAPTION "SCREEN$ editor V%d.%d"

#define SCREENWIDTH 800
//#define SCREENHEIGHT 384
#define SCREENHEIGHT 600

extern char *screen_name;

extern int debug;
extern int attr_debug;
extern unsigned char zxscr[];
extern unsigned char attrs[];
extern unsigned int colour_palette[];
extern unsigned char characters[96][32];
extern unsigned char square_draw[];
extern unsigned char flash_squares[];

extern SDL_Surface *scrbuf;
extern int bgcol;
extern int gridcol;
extern int screen_is_modif;
extern int ask;
extern int want_exit;
extern int want_new_file;

extern int small_scr;
extern int big_scr;

extern int xofpalette;
extern int yofpalette;

extern int xofattrs;
extern int yofattrs;

extern int xofcell;
extern int yofcell;

extern int line;
extern int col;

extern int new_ink;
extern int new_paper;
extern int new_bright;
extern int new_flash;

extern Uint32 pulse;

extern int grid_is_visible;
extern int attr;
extern int cell_mem_addr;
extern char cell_pixels[8][8];

extern unsigned char flash_state;
extern int flash_pause;

extern int finished_load;
extern int attributes_done;

extern int loading_x;
extern int loading_y;
extern int le_octet_video;
extern int line_inc;
extern int zone_inc;




// Icons:
extern unsigned char help[];
extern const unsigned long help_size;
extern unsigned char stop[];
extern const unsigned long stop_size;
extern unsigned char save[];
extern const unsigned long save_size;
extern unsigned char newfile[];
extern const unsigned long newfile_size;
extern unsigned char warn[];
extern const unsigned long warn_size;
extern unsigned char ok[];
extern const unsigned long ok_size;
extern unsigned char cancel[];
extern const unsigned long cancel_size;
extern unsigned char info[];
extern const unsigned long info_size;
extern unsigned char tape[];
extern const unsigned long tape_size;



// Functions:
int main (int, char *[]);
void new_empty_file (void);
int init_graphic_lib (void);
int init_graphic_window (void);
int close_graphic_lib (void);
int clear_rect (int, int, int, int, int);
int putpixel (SDL_Surface *, int, int, Uint32);
int update_screen (void);
int handle_events (void);
void ask_for_save (void);
void show_icon (int, int, int, int, int);
void draw_icons (void);
int draw_screen (void);
int draw_screen_pixels (int, int, int, int, int);
int get_ink_by_attr (int);
int get_paper_by_attr (int);
//int get_bright_by_attr (int);
int get_flash_by_attr (int);
int draw_grid (int, int, int, int, int);
int draw_colour_palette (int, int);
int draw_cell (int, int);
int show_current_selected_attribure (int, int, int, int);
int printstring(unsigned char *, int, int, unsigned char, unsigned char);
int printchar(unsigned char *, unsigned char, int, int, unsigned char, unsigned char, int);
void move_right (void);
void move_left (void);
void move_up (void);
void move_down (void);
void redraw_all (void);
void mouse_button_release_left (int, int);
void mouse_button_release_right (int, int);
int make_attr ( int, int);
void save_screen (void);
void about (void);
void do_flash (void);
void set_new_flash (void);
void get_new_attrs_of_cell(int, int);
int get_me_attrnr_of_cell (int, int);
void finish_move (void);
void loading_efect (void);
void put_attributes (void);
void text_scroll_efect (void);

#endif /* MAIN_H */
