// ZX-Spectrum SCREEN$ editor
// (c) 2004 Catalin Mihaila <mihaila_ac@yahoo.com>

#include "main.h"

// icons numbers:
// 1. help icon
// 2. stop icon
// 3. save icon
// 4. new icon
// 5. warn icon
// 6. ok icon
// 7. cancel icon
// 8. info icon

void draw_icons (void)
{
    if(debug) { printf("Draw icons:\n"); }
    show_icon(1, SCREENWIDTH-32, SCREENHEIGHT-32, 32, 32); // 
    show_icon(2, SCREENWIDTH-64, SCREENHEIGHT-32, 32, 32); // 
    show_icon(3, SCREENWIDTH-96, SCREENHEIGHT-32, 32, 32); // 
    show_icon(4, SCREENWIDTH-128, SCREENHEIGHT-32, 32, 32); // 
}

void show_icon (int iconnr, int icon_x, int icon_y, int icon_w, int icon_h)
{
    int x_of_icon = icon_x;
    int y_of_icon = icon_y;
    int icon_size = 0;
    int index;
    Uint32 iconpixel = 0;

    if(iconnr==1) icon_size = help_size;
    if(iconnr==2) icon_size = stop_size;
    if(iconnr==3) icon_size = save_size;
    if(iconnr==4) icon_size = newfile_size;
    if(iconnr==5) icon_size = warn_size;
    if(iconnr==6) icon_size = ok_size;
    if(iconnr==7) icon_size = cancel_size;
    if(iconnr==8) icon_size = info_size;

    if(debug) { printf("Draw icon number %d\n",iconnr); }

if(!icon_size) return;
    for(index=0;index<icon_size;index++)
	{
	    if(iconnr==1) iconpixel = SDL_MapRGB(scrbuf->format, help[index], help[index+1], help[index+2]);
	    if(iconnr==2) iconpixel = SDL_MapRGB(scrbuf->format, stop[index], stop[index+1], stop[index+2]);
	    if(iconnr==3) iconpixel = SDL_MapRGB(scrbuf->format, save[index], save[index+1], save[index+2]);
	    if(iconnr==4) iconpixel = SDL_MapRGB(scrbuf->format, newfile[index], newfile[index+1], newfile[index+2]);
	    if(iconnr==5) iconpixel = SDL_MapRGB(scrbuf->format, warn[index], warn[index+1], warn[index+2]);
	    if(iconnr==6) iconpixel = SDL_MapRGB(scrbuf->format, ok[index], ok[index+1], ok[index+2]);
	    if(iconnr==7) iconpixel = SDL_MapRGB(scrbuf->format, cancel[index], cancel[index+1], cancel[index+2]);
	    if(iconnr==8) iconpixel = SDL_MapRGB(scrbuf->format, info[index], info[index+1], info[index+2]);

	    putpixel(scrbuf, x_of_icon, y_of_icon, iconpixel);
	    x_of_icon++;
	    if(x_of_icon == icon_x + icon_w) {x_of_icon=icon_x; y_of_icon++;}
	    index+=2;
	}
}
