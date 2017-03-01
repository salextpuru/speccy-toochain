// ZX-Spectrum SCREEN$ editor
// (c) 2004 Catalin Mihaila <mihaila_ac@yahoo.com>

#include "main.h"

SDL_Event event;

int want_exit       = 0;



// wait for one event:
int handle_events (void)
{
	int filenew;

    while( SDL_PollEvent(&event))
        {
            switch( event.type )
                {
                    case SDL_QUIT:
						if(screen_is_modif)
							{
								want_exit = 1;
								small_scr = 0;
								ask_for_save();
								return 1;
							} else {
								return 0;
							}
					break;

		    case SDL_KEYUP:
			switch(event.key.keysym.sym)
			    {
					case SDLK_ESCAPE:
					case SDLK_F10:
						if(screen_is_modif)
				    		{
								want_exit = 1;
								small_scr = 0;
								ask_for_save();
								return 1;
				    		} else {
								return 0;
				    		}
					break;

					case SDLK_F1:
						small_scr = 0;
						about();
						ask = 0;
					break;

					case SDLK_F2:
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
					break;

					case SDLK_F3:
						save_screen();
					break;

					case SDLK_F5:
					case SDLK_g:
					    if(grid_is_visible) { grid_is_visible = 0; }
						else { grid_is_visible = 1; }
						ask = 0;
						redraw_all();
					break;

					case SDLK_RIGHT:
			    		move_right();
			    	break;

					case SDLK_LEFT:
			    		move_left();
			    	break;

					case SDLK_DOWN:
			    		move_down();
			    	break;

					case SDLK_UP:
			    		move_up();
					break;

case SDLK_FIRST:
case SDLK_BACKSPACE:
case SDLK_TAB:
case SDLK_CLEAR:
case SDLK_RETURN:
case SDLK_PAUSE:
case SDLK_SPACE:
case SDLK_EXCLAIM:
case SDLK_QUOTEDBL:
case SDLK_HASH:
case SDLK_DOLLAR:
case SDLK_AMPERSAND:
case SDLK_QUOTE:
case SDLK_LEFTPAREN:
case SDLK_RIGHTPAREN:
case SDLK_ASTERISK:
case SDLK_PLUS:
case SDLK_COMMA:
case SDLK_MINUS:
case SDLK_PERIOD:
case SDLK_SLASH:
case SDLK_0:
case SDLK_1:
case SDLK_2:
case SDLK_3:
case SDLK_4:
case SDLK_5:
case SDLK_6:
case SDLK_7:
case SDLK_8:
case SDLK_9:
case SDLK_COLON:
case SDLK_SEMICOLON:
case SDLK_LESS:
case SDLK_EQUALS:
case SDLK_GREATER:
case SDLK_QUESTION:
case SDLK_AT:
case SDLK_LEFTBRACKET:
case SDLK_BACKSLASH:
case SDLK_RIGHTBRACKET:
case SDLK_CARET:
case SDLK_UNDERSCORE:
case SDLK_BACKQUOTE:
case SDLK_a:
case SDLK_b:
case SDLK_c:
case SDLK_d:
case SDLK_e:
case SDLK_f:
case SDLK_h:
case SDLK_i:
case SDLK_j:
case SDLK_k:
case SDLK_l:
case SDLK_m:
case SDLK_n:
case SDLK_o:
case SDLK_p:
case SDLK_q:
case SDLK_r:
case SDLK_s:
case SDLK_t:
case SDLK_u:
case SDLK_v:
case SDLK_w:
case SDLK_x:
case SDLK_y:
case SDLK_z:
case SDLK_DELETE:
case SDLK_WORLD_0:
case SDLK_WORLD_1:
case SDLK_WORLD_2:
case SDLK_WORLD_3:
case SDLK_WORLD_4:
case SDLK_WORLD_5:
case SDLK_WORLD_6:
case SDLK_WORLD_7:
case SDLK_WORLD_8:
case SDLK_WORLD_9:
case SDLK_WORLD_10:
case SDLK_WORLD_11:
case SDLK_WORLD_12:
case SDLK_WORLD_13:
case SDLK_WORLD_14:
case SDLK_WORLD_15:
case SDLK_WORLD_16:
case SDLK_WORLD_17:
case SDLK_WORLD_18:
case SDLK_WORLD_19:
case SDLK_WORLD_20:
case SDLK_WORLD_21:
case SDLK_WORLD_22:
case SDLK_WORLD_23:
case SDLK_WORLD_24:
case SDLK_WORLD_25:
case SDLK_WORLD_26:
case SDLK_WORLD_27:
case SDLK_WORLD_28:
case SDLK_WORLD_29:
case SDLK_WORLD_30:
case SDLK_WORLD_31:
case SDLK_WORLD_32:
case SDLK_WORLD_33:
case SDLK_WORLD_34:
case SDLK_WORLD_35:
case SDLK_WORLD_36:
case SDLK_WORLD_37:
case SDLK_WORLD_38:
case SDLK_WORLD_39:
case SDLK_WORLD_40:
case SDLK_WORLD_41:
case SDLK_WORLD_42:
case SDLK_WORLD_43:
case SDLK_WORLD_44:
case SDLK_WORLD_45:
case SDLK_WORLD_46:
case SDLK_WORLD_47:
case SDLK_WORLD_48:
case SDLK_WORLD_49:
case SDLK_WORLD_50:
case SDLK_WORLD_51:
case SDLK_WORLD_52:
case SDLK_WORLD_53:
case SDLK_WORLD_54:
case SDLK_WORLD_55:
case SDLK_WORLD_56:
case SDLK_WORLD_57:
case SDLK_WORLD_58:
case SDLK_WORLD_59:
case SDLK_WORLD_60:
case SDLK_WORLD_61:
case SDLK_WORLD_62:
case SDLK_WORLD_63:
case SDLK_WORLD_64:
case SDLK_WORLD_65:
case SDLK_WORLD_66:
case SDLK_WORLD_67:
case SDLK_WORLD_68:
case SDLK_WORLD_69:
case SDLK_WORLD_70:
case SDLK_WORLD_71:
case SDLK_WORLD_72:
case SDLK_WORLD_73:
case SDLK_WORLD_74:
case SDLK_WORLD_75:
case SDLK_WORLD_76:
case SDLK_WORLD_77:
case SDLK_WORLD_78:
case SDLK_WORLD_79:
case SDLK_WORLD_80:
case SDLK_WORLD_81:
case SDLK_WORLD_82:
case SDLK_WORLD_83:
case SDLK_WORLD_84:
case SDLK_WORLD_85:
case SDLK_WORLD_86:
case SDLK_WORLD_87:
case SDLK_WORLD_88:
case SDLK_WORLD_89:
case SDLK_WORLD_90:
case SDLK_WORLD_91:
case SDLK_WORLD_92:
case SDLK_WORLD_93:
case SDLK_WORLD_94:
case SDLK_WORLD_95:
case SDLK_KP0:
case SDLK_KP1:
case SDLK_KP2:
case SDLK_KP3:
case SDLK_KP4:
case SDLK_KP5:
case SDLK_KP6:
case SDLK_KP7:
case SDLK_KP8:
case SDLK_KP9:
case SDLK_KP_PERIOD:
case SDLK_KP_DIVIDE:
case SDLK_KP_MULTIPLY:
case SDLK_KP_MINUS:
case SDLK_KP_PLUS:
case SDLK_KP_ENTER:
case SDLK_KP_EQUALS:
case SDLK_INSERT:
case SDLK_HOME:
case SDLK_END:
case SDLK_PAGEUP:
case SDLK_PAGEDOWN:
case SDLK_F4:
case SDLK_F6:
case SDLK_F7:
case SDLK_F8:
case SDLK_F9:
case SDLK_F11:
case SDLK_F12:
case SDLK_F13:
case SDLK_F14:
case SDLK_F15:
case SDLK_NUMLOCK:
case SDLK_CAPSLOCK:
case SDLK_SCROLLOCK:
case SDLK_RSHIFT:
case SDLK_LSHIFT:
case SDLK_RCTRL:
case SDLK_LCTRL:
case SDLK_RALT:
case SDLK_LALT:
case SDLK_RMETA:
case SDLK_LMETA:
case SDLK_LSUPER:
case SDLK_RSUPER:
case SDLK_MODE:
case SDLK_COMPOSE:
case SDLK_HELP:
case SDLK_PRINT:
case SDLK_SYSREQ:
case SDLK_BREAK:
case SDLK_MENU:
case SDLK_POWER:
case SDLK_EURO:
case SDLK_UNDO:
case SDLK_LAST:
break;
			    }
		    break;

			case SDL_MOUSEBUTTONUP:
			if(event.button.button == SDL_BUTTON_LEFT)
			    {
// click on exit icon:
				if( event.button.x > SCREENWIDTH-65)
				    {
					if( event.button.x < SCREENWIDTH-32)
					    {
						if( event.button.y > SCREENHEIGHT-33)
						    {
							if(screen_is_modif)
							    {
								want_exit = 1;
								small_scr = 0;
								ask_for_save();
								return 1;
							    } else {
								return 0;
							    }
						    }
					    }
				    }
				mouse_button_release_left(event.button.x, event.button.y);
			    }
			if(event.button.button == SDL_BUTTON_RIGHT) { mouse_button_release_right(event.button.x, event.button.y); }
                    break;

		    case SDL_KEYDOWN:
            break;

            default:
            break;
                }
        }
    return 1;
}
