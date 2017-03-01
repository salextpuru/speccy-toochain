// ZX-Spectrum SCREEN$ editor
// (c) 2004 Catalin Mihaila <mihaila_ac@yahoo.com>

#include "main.h"

// mouse button is released
void mouse_button_release_left (int moux, int mouy)
{
    int pixline        = 0;
    int pixcol         = 0;
    int new_octet      = 0;
    int screenpix      = 0;
    int attr_oct       = 0;
    int attr_val       = 0;
	int filenew;


// answer the question:
    if(ask)
	{

// OK button:
	    if((moux > (SCREENWIDTH-193)) &&
	       (moux < (SCREENWIDTH-159)) &&
	       (mouy > 151) &&
	       (mouy < 185))
		{
		    save_screen();
		    if(want_exit) {
			close_graphic_lib();
			exit(0);
		    } else {
			if(want_new_file)
				{
					for(filenew=0;filenew<768;filenew++) square_draw[filenew] = 0;
					clear_rect(0,SCREENHEIGHT-192,256,192,7);
					new_empty_file();
				}
			redraw_all();
		    }
		}

// CANCEL button:
	    if((moux > (SCREENWIDTH-97)) &&
	       (moux < (SCREENWIDTH-63)) &&
	       (mouy > 151) &&
	       (mouy < 185))
		{
		    if(want_exit) {
			close_graphic_lib();
			exit(0);
		    } else {
			if(want_new_file)
				{
					for(filenew=0;filenew<768;filenew++) square_draw[filenew] = 0;
					clear_rect(0,SCREENHEIGHT-192,256,192,7);
					new_empty_file();
				}
			redraw_all();
		    }
		}
	} // end if_ask

// click on bright rect:
    if((moux > (xofattrs+107)) &&
       (moux < (xofattrs+125)) &&
       (mouy > (yofattrs+39))  &&
       (mouy < (yofattrs+57)))
	{
	    attr_oct = get_me_attrnr_of_cell (line, col);
	    attr_val = attrs[attr_oct];

	    if(attr_val>>0x06&0x01)
		{
		    if((new_ink > 7) && (new_paper > 7)) { new_ink-=8; new_paper-=8; } // want to be sure!
		    attrs[attr_oct]=attr_val-64;
		} else {
		    if((new_ink < 8) && (new_paper < 8)) { new_ink+=8; new_paper+=8; } // want to be sure!
		    attrs[attr_oct]=attr_val+64;
		}

	    screen_is_modif = 1;
	    ask = 0;
	    redraw_all();
	}


// click on flash rect:
    if((moux > (xofattrs+107)) &&
       (moux < (xofattrs+125)) &&
       (mouy > (yofattrs+59))  &&
       (mouy < (yofattrs+77)))
	{
	    attr_oct = get_me_attrnr_of_cell (line, col);
	    attr_val = attrs[attr_oct];

	    if(new_flash) {
		if( attr_val > 128) { attrs[attr_oct]=attr_val-128; new_flash=0; }
	    } else {
		if( attr_val < 128) { attrs[attr_oct]=attr_val+128; new_flash=1; }
	    }

	    screen_is_modif = 1;
	    ask = 0;
	    redraw_all();
	}

// click on grid rect:
    if((moux > (xofattrs+107)) &&
       (moux < (xofattrs+125)) &&
       (mouy > (yofattrs+79))  &&
       (mouy < (yofattrs+97)))
	{
	    if(grid_is_visible) { grid_is_visible = 0; }
	    else { grid_is_visible = 1; }
	    ask = 0;
	    redraw_all();
	}


// click on newfile icon:
    if((moux > (SCREENWIDTH-129)) &&
       (moux < (SCREENWIDTH-96))  &&
       (mouy > (SCREENHEIGHT-33)))
	{
	    want_exit = 0;
	    if(screen_is_modif)
		{
		    want_new_file = 1;
		    small_scr = 0;
		    ask_for_save();
		} else {
			for(filenew=0;filenew<768;filenew++) square_draw[filenew] = 0;
			clear_rect(0,SCREENHEIGHT-192,256,192,7);
		    new_empty_file();
		    redraw_all();
		}
	}

// click on save icon:
    if((moux > (SCREENWIDTH-97)) &&
       (moux < (SCREENWIDTH-64)) &&
       (mouy > (SCREENHEIGHT-33)))
	{
	    save_screen();
	}


// click on ? icon:
    if((moux > SCREENWIDTH-33) && (mouy > SCREENHEIGHT-33))
	{
	    small_scr = 0;
	    about();
	    ask = 0;
	}


// click on big screen:
    if((moux < 513) && (mouy < 385))
	{
	    col = moux/16;
	    line = mouy/16;
	    small_scr = 1;
	    ask = 0;
	    get_new_attrs_of_cell(line, col);
	    redraw_all();
	}

// click on cell: draw a new cell with selected attr:
    if((mouy > 213) && (mouy < 343))
	{
	    if(moux > xofcell-1)
		{

		    small_scr = 1;

// which colon of pixels from cell ??
		    pixcol = ((moux-SCREENWIDTH+128)/16);

// which line of pixels from cell ??
		    pixline = ((mouy-yofcell)/16);

		    cell_pixels[pixline][7-pixcol] = 1;

		    new_octet =(      cell_pixels[pixline][0]
		                +(  2*cell_pixels[pixline][1])
				+(  4*cell_pixels[pixline][2])
				+(  8*cell_pixels[pixline][3])
				+( 16*cell_pixels[pixline][4])
				+( 32*cell_pixels[pixline][5])
				+( 64*cell_pixels[pixline][6])
				+(128*cell_pixels[pixline][7])
			       );

		    screenpix = (256*pixline)+cell_mem_addr;

		    zxscr[screenpix] = new_octet;

		    if(get_flash_by_attr(attrs[attr])) {
		    attrs[attr] = make_attr(new_ink, new_paper)+128;
		    } else {
		    attrs[attr] = make_attr(new_ink, new_paper);
		    }

		    screen_is_modif = 1;
		    ask = 0;
		    redraw_all();
		}
	}


// click on palette: select ink from palette
    if((mouy > (yofpalette-1)) && (mouy < (yofpalette+17)))
	{
	    if(moux > (xofpalette-1))
		{
		    new_ink = ((moux-xofpalette)/16);

// set correct bright for cell:
		    if((new_ink > 7) && (new_paper < 8)) { new_paper+=8; }
		    if((new_ink < 8) && (new_paper > 7)) { new_paper-=8; }

		    show_current_selected_attribure(xofattrs, yofattrs, new_ink, new_paper);
		    ask = 0;
		    update_screen();
		}
	}
}

void mouse_button_release_right (int moux, int mouy)
{
    int pixline = 0;
    int pixcol = 0;
    int new_octet = 0;
    int screenpix = 0;

// click on cell: draw a new cell with paper:
    if((mouy > 213) && (mouy < 343))
	{
	    if(moux > xofcell-1)
		{
// which colon of pixels from cell ??
		    pixcol = ((moux-SCREENWIDTH+128)/16);

// which line of pixels from cell ??
		    pixline = ((mouy-yofcell)/16);

		    cell_pixels[pixline][7-pixcol] = 0;

		    new_octet =(      cell_pixels[pixline][0]
		                +(  2*cell_pixels[pixline][1])
				+(  4*cell_pixels[pixline][2])
				+(  8*cell_pixels[pixline][3])
				+( 16*cell_pixels[pixline][4])
				+( 32*cell_pixels[pixline][5])
				+( 64*cell_pixels[pixline][6])
				+(128*cell_pixels[pixline][7])
			       );

		    screenpix = (256*pixline)+cell_mem_addr;

		    zxscr[screenpix] = new_octet;
		    if(get_flash_by_attr(attrs[attr])) {
		    attrs[attr] = make_attr(new_ink, new_paper)+128;
		    } else {
		    attrs[attr] = make_attr(new_ink, new_paper);
		    }
		    screen_is_modif = 1;
		    ask = 0;
		    redraw_all();
		}
	}


// click on palette: select paper from palette
    if((mouy > (yofpalette-1)) && mouy < ((yofpalette+17)))
	{
	    if(moux > (xofpalette-1))
		{
		    new_paper = ((moux-xofpalette)/16);

// set correct bright for cell:
		    if((new_paper > 7) && (new_ink < 8)) { new_ink+=8; new_bright=1; }
		    if((new_paper < 8) && (new_ink > 7)) { new_ink-=8; new_bright=0;}

		    show_current_selected_attribure(xofattrs, yofattrs, new_ink, new_paper);
		    ask = 0;
		    update_screen();
		}
	}
}
