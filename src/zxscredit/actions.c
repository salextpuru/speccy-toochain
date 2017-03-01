// ZX-Spectrum SCREEN$ editor
// (c) 2004 Catalin Mihaila <mihaila_ac@yahoo.com>

#include "main.h"

int parity=255;

void ask_for_save (void)
{
    clear_rect((SCREENWIDTH-256), 0, 256, 192, 7);
    show_icon(5, SCREENWIDTH-250, 2, 32, 32);
    printstring("SCREEN$ is not", (SCREENWIDTH-250), 34, 0, 7);
    printstring("saved...", (SCREENWIDTH-250), 52, 0, 7);
    printstring("Save now ?", (SCREENWIDTH-200), 86, 0, 7);
    show_icon(6, SCREENWIDTH-192, 152, 32, 32);
    show_icon(7, SCREENWIDTH-96, 152, 32, 32);
    ask = 1;
    update_screen();
}

void save_screen (void)
{
    FILE *fisier;
    int par;

    if(screen_name != NULL) {
    fisier=fopen(screen_name,"wb");
    } else {
    fisier=fopen("zxscrout.scr","wb");
	}

    fwrite(&zxscr[0], 6144,  1,  fisier);
    fwrite(&attrs[0], 768,  1,  fisier);

    fclose(fisier);

    for(par=0;par<6144;par++) parity = parity ^ zxscr[par];
    for(par=0;par<768;par++) parity = parity ^ attrs[par];

    fisier=fopen("zxscrout.tap","wb");
    fwrite(&tape[0],tape_size, 1, fisier);
    fwrite(&zxscr[0], 6144,  1,  fisier);
    fwrite(&attrs[0], 768,  1,  fisier);
    putc(parity,fisier);
    fclose(fisier);

    screen_is_modif = 0;
    ask = 0;
    redraw_all();
}

void about (void)
{
    char ptrbuff[256];
    clear_rect((SCREENWIDTH-256), 0, 256, 192, 7);
    show_icon(8, SCREENWIDTH-250, 2,32,32);
    sprintf(ptrbuff,"ZXscredit V%d.%d",MAJV,MINV);
    printstring(ptrbuff, (SCREENWIDTH-240), 50, 0, 7);
    printstring("Written by:", (SCREENWIDTH-250), 100, 0, 7);
    printstring("Catalin Mihaila", (SCREENWIDTH-248), 125, 0, 7);
    update_screen();
}
