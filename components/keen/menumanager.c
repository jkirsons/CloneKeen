
// functions to handle overlay menus (like the main menu)
// these are menus which are overlaid over the game
// as it's playing (or a demo is running, same thing)
// I think of them as "Quake-style" menus.

#include <SDL_input.h>			// for SDLK_ keys
#include "keen.h"
#include "menumanager.fdh"
#include "menu.h"

int Menu_HandleCount = 0;
char tempbuf[80];
stCurMenu menu;

uchar *main_menu[MAXM] = {88,79,  0, NULL, NULL, NULL, NULL,
					"1-Player Game", MNU_CALLFP, goto_epmenu, 1,
					"2-Player Game", MNU_CALLFP, goto_epmenu, 2,
					"Editor", MNU_SWITCHMENU, edloadmap_menu, 0,
					"Load Game", MNU_SWITCHMENU, loadgame_menu, 0,
					"Options", MNU_SWITCHMENU, option_menu, 0,
					"About", MNU_SWITCHMENU, about_menu, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"Quit", MNU_CALLFP, mmhandler_quit, 0,
					NULL, NULL, NULL, 0
					};
					
uchar *ingame_menu[MAXM] = {88,79,  0, NULL, NULL, NULL, NULL,
					"New Game", MNU_SWITCHMENU, selectep_menu, 0,
					"Save Game", MNU_SWITCHMENU, savegame_menu, 0,
					"Load Game", MNU_SWITCHMENU, loadgame_menu, 0,
					"Options", MNU_SWITCHMENU, option_menu, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"End Level", MNU_CALLFP, mmhandler_restartlevel, 2,
					"End Game", MNU_SWITCHMENU, endgame_menu, 0,
					"Quit", MNU_CALLFP, mmhandler_quit, 0,
					NULL, NULL, NULL, 0
					};
					
uchar *ingame_menu_norestart[MAXM] = {88,79,  0, NULL, NULL, NULL, NULL,
					"New Game", MNU_SWITCHMENU, selectep_menu, 0,
					"Save Game", MNU_SWITCHMENU, savegame_menu, 0,
					"Load Game", MNU_SWITCHMENU, loadgame_menu, 0,
					"Options", MNU_SWITCHMENU, option_menu, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"End Game", MNU_SWITCHMENU, endgame_menu, 0,
					"Quit", MNU_CALLFP, mmhandler_quit, 0,
					NULL, NULL, NULL, 0
					};
					
uchar *endgame_menu[MAXM] = {90,100,  0, NULL, NULL, NULL, NULL,
					"End the Game", MNU_CALLFP, mmhandler_endgame, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"Play on!", MNU_CALLFP, dismiss_menu, 0,
					NULL, NULL, NULL, 0
					};
					
uchar *quit_menu[MAXM] = {90,100, 0, NULL, NULL, NULL, NULL,
					"Quit the Program", MNU_CALLFP, mmhandler_quit2, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"Play on!", MNU_CALLFP, dismiss_menu, 0,
					NULL, NULL, NULL, 0
					};
					
uchar *selectep_menu[MAXM] = {5*8,90,  0, NULL, NULL, NULL, NULL,
					"Invasion of the Vorticons", MNU_CALLFP, mmhandler_newgame, 1,
					"The Earth Explodes", MNU_CALLFP, mmhandler_newgame, 2,
					"Keen Must Die!", MNU_CALLFP, mmhandler_newgame, 3,
					"", MNU_SEPERATOR, NULL, 0,
					"Load User Map", MNU_SWITCHMENU, custommap_menu, 0,
					"Back to Main Menu", MNU_SWITCHMENU, main_menu, 1,
					NULL, NULL, NULL, 0
					};
					
uchar *about_menu[MAXM] = {-3,60,  0xff, NULL, NULL, NULL, NULL,
					REVISION, MNU_SEPERATOR, NULL, 0,
					"by Caitlin Shaw", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,
					PORTEDBY1, MNU_SEPERATOR, NULL, 0,
					PORTEDBY2, MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"Based on \"Invasion of the Vorticons\"", MNU_SEPERATOR, NULL, 0,
					"by ID Software. All \"Commander Keen\"", MNU_SEPERATOR, NULL, 0,
					"datafiles are (c)1991 ID Software.", MNU_SEPERATOR, NULL, 0,
					"CloneKeen is an original game engine", MNU_SEPERATOR, NULL, 0,
					"which is compatible with these data", MNU_SEPERATOR, NULL, 0,
					"files.", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"This program is free software under", MNU_SEPERATOR, NULL, 0,
					"the GNU General Public License.", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"GR00VY DUDE!", MNU_SWITCHMENU, main_menu, 0
					};
					
uchar *cant_play_ep_menu[MAXM] = {23,74,  255, NULL, NULL, NULL, NULL,
					"You can't play this episode.", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"You are missing the data files", MNU_SEPERATOR, NULL, 0,
					"from the original game, which", MNU_SEPERATOR, NULL, 0,
					"you need to play it.", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"Obtain the original version of", MNU_SEPERATOR, NULL, 0,
					"this episode, copy all it's", MNU_SEPERATOR, NULL, 0,
					"files into the \"data\" subdir,", MNU_SEPERATOR, NULL, 0,
					"and restart CloneKeen.", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"Aw, shucks, ok.", MNU_SWITCHMENU, selectep_menu, 0,
					};
					
uchar *cant_play_usermap_menu[MAXM] = {23,74,  255, NULL, NULL, NULL, NULL,
					"You can't play this user map.", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"This user map requires the", MNU_SEPERATOR, NULL, 0,
					"registered version of Keen.", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"Obtain the original version of", MNU_SEPERATOR, NULL, 0,
					"the episode, copy all it's", MNU_SEPERATOR, NULL, 0,
					"files into the \"data\" subdir,", MNU_SEPERATOR, NULL, 0,
					"and restart CloneKeen.", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"Aw, shucks, ok.", MNU_SWITCHMENU, main_menu, 0,
					};
					
// call during game startup so DismissalFunc doesn't blow up first time
void Menu_Init(void)
{
	menu.curmenu = NULL;
	menu.visible = 0;
}

// return whether the onscreen menu is visible
char Menu_IsVisible(void)
{
	return menu.visible;
}
void Menu_SetVisibility(int newvis)
{
	if (menu.visible != newvis)
	{
		menu.visible = newvis;
		if (!newvis) Menu_CallDismissalFunc();
	}
}

void static SelectCorrectMM(void)
{
	if (editor)
	{
		if (menu.curmenu != editor_menu)
			Menu_InitMenu(editor_menu);
		return;
	}
	
	if (levelcontrol.gameinprogress)
	{
		if (game_is_over() || levelcontrol.curlevel==WORLD_MAP || \
			cinematic_is_playing() || levelcontrol.vibratetime > 0 ||\
			message_up())
		{
			if (menu.curmenu != ingame_menu_norestart)
			{
				Menu_InitMenu(ingame_menu_norestart);
			}
		}
		else
		{
			if (menu.curmenu != ingame_menu)
			{
				Menu_InitMenu(ingame_menu);
			}
		}
	}
	else if (menu.curmenu != main_menu)
		Menu_InitMenu(main_menu);
}
	
// draws the dialog box, the menu options, and the twirly
void static DrawMenuText(void)
{
int x,y,optno,index;
uchar *curmenu = menu.curmenu;
char *(*dyn_ptr)(uchar *MenuText, char arg, uchar param);
char *text;

	// if on main menu ensure correct version is up
	if (menu.curmenu==main_menu || \
		menu.curmenu==ingame_menu || \
		menu.curmenu==ingame_menu_norestart)
	{
		SelectCorrectMM();
	}
	
	x = (*menu.curmenu)[HD_X];
	y = (*menu.curmenu)[HD_Y];
	// draw the text
	x += (3*8); index = M_HEADERLEN; optno = 0; y+=8;
	while(1)
	{
		if (!(*menu.curmenu)[index]) break;
		
		text = (*menu.curmenu)[index];
		if ((*menu.curmenu)[index+ITM_TYPE] == MNU_DYNAMICTEXT)
		{
			dyn_ptr = (*menu.curmenu)[index+ITM_PTR];
			text = (*dyn_ptr)(text, 0, (*menu.curmenu)[index+ITM_FLAGS]);
		}
		font_draw(text, x, y, drawcharacter_clear);
		
		if (optno==menu.curoption)
		{		// draw twirly marker
			twirly_draw((*menu.curmenu)[HD_X]+8, y+menu.twirly_yoff,
					    &menu.curtwirlframe);
		}
			
		index += M_ENTRYLEN;
		optno++;
		y += 9;
	}
}

// call the menu's "dismissal" function
void Menu_CallDismissalFunc(void)
{
void (*funcptr)(void);
	if (menu.curmenu)
	{
		funcptr = (*menu.curmenu)[HD_DISMISSALFUNC];
		menu.curmenu = NULL;		// must be here to prevent recusive loop sometimes
		if (funcptr) (*funcptr)();
	}
}

// initilize to draw a menu
char firsttime = 1;
void Menu_InitMenu(uchar *whichmenu)
{
int index;
void (*funcptr)(void);

	Menu_CallDismissalFunc();
	
	menu.curmenu = whichmenu;
	menu.curoption = (*menu.curmenu)[HD_STARTINGOPT];
	twirly_init(&menu.curtwirlframe, &menu.twirltimer);
	menu.twirly_yoff = 0;
	menu.twirly_yoff_vector = 0;
	menu.lastkeydir = KENTER;
	menu.visible = 1;
	
	// count # of options in menu
	menu.last_option = 0; index = M_HEADERLEN;
	while((*menu.curmenu)[index])
	{
		menu.last_option++;
		index += 4;
	}
	menu.last_option--;
	
	if (menu.curoption==0xff)
	 { menu.curoption = menu.last_option; menu.cantmove = 1; }
	else
	 { menu.cantmove = 0; }
	
	// execute the "on-enter" function
	funcptr = (*menu.curmenu)[HD_ENTERFUNC];
	if (funcptr) (*funcptr)();
}

void Menu_ToMain(void)
{
	Menu_InitMenu(main_menu);
}

// select ("activate") the option "opt" in current menu
void Menu_SelectOption(int opt)
{
int index = M_HEADERLEN+(menu.curoption*M_ENTRYLEN); // get index of selected menu opt
uchar action = (*menu.curmenu)[index+ITM_TYPE];
uchar arg = (*menu.curmenu)[index+ITM_FLAGS];
char *newmenu;
char (*funcptr)(int arg);
char (*dyn_ptr)(char *MenuText, char arg, uchar param);
int closemenu;


	switch(action)
	{
		case MNU_SWITCHMENU:
			newmenu = (*menu.curmenu)[index+ITM_PTR];
			Menu_InitMenu(newmenu);
		break;
		
		case MNU_CALLFP:
			funcptr = (*menu.curmenu)[index+ITM_PTR];
			closemenu = (*funcptr)(arg);
			if (closemenu)
			{
				menu.visible = 0;
			}
		break;
		
		case MNU_DYNAMICTEXT:
			dyn_ptr = (*menu.curmenu)[index+ITM_PTR];
			(*dyn_ptr)((*menu.curmenu)[index], 1, (*menu.curmenu)[index+ITM_FLAGS]);
		break;
		
		default:
			crash("invalid action %d in Menu_SelectOption\n", action);
	}
}

void static DrawLogo(void)
{
int title_wd, title_x;
	// draw "CloneKeen" title
	title_wd = (sprites[TITLE_FLOOR_SPRITE].xsize * 2);
	title_x = ((WINDOW_WIDTH-title_wd) >> 1) - 1;
	#define TITLE_Y		2
	
	drawsprite(title_x+12, TITLE_Y, TITLE_LOGO1_SPRITE);
	drawsprite(title_x+12+sprites[TITLE_LOGO1_SPRITE].xsize, TITLE_Y, TITLE_LOGO2_SPRITE);
	drawsprite(title_x+40, (TITLE_Y+45)-sprites[PFIREFRAME].ysize, PFIREFRAME);
	drawsprite(title_x+71, TITLE_Y+30, objdefsprites[OBJ_RAY]);
	drawsprite(title_x+98, TITLE_Y+13, TITLE_VORT_SPRITE);
	drawsprite(title_x, TITLE_Y+45, TITLE_FLOOR_SPRITE);
	drawsprite(title_x+80, TITLE_Y+45, TITLE_FLOOR_SPRITE);
}

// draw and handle the currently "up" menu
void Menu_DrawMenu(void)
{
	if (!menu.visible) return;
	
	if (menu.curmenu != option_menu)
	{
		DrawLogo();
	}
	
	DrawMenuText();
}

// return the type (ITM_TYPE, e.g. MNU_SEPERATOR) of a given menu option
int menu_gettype(int opt)
{
int index = (opt*M_ENTRYLEN)+M_HEADERLEN;
	return (*menu.curmenu)[index+ITM_TYPE];
}

// handles moving the twirly up and down
void static move_down(void)
{
char (*scrollfunc)(char arg);
int i,canmove;

	canmove = 0;
	if (menu.curoption < menu.last_option)
	{
		for(i=menu.curoption+1;i<=menu.last_option;i++)
		{
			if (menu_gettype(i) != MNU_SEPERATOR) { canmove = 1; break; }
		}
	}
	
	if (canmove)
	{
		menu.twirly_yoff_vector = 1;
	}
	else
	{
		scrollfunc = (*menu.curmenu)[HD_SCROLLFUNC];
		if (scrollfunc)
			(*scrollfunc)(DOWN);
		else
		{
			menu.curoption = 0;
			while(menu_gettype(menu.curoption) == MNU_SEPERATOR) menu.curoption++;
		}
	}
}
void static move_up(void)
{
char (*scrollfunc)(char arg);
int i,canmove;

	canmove = 0;
	if (menu.curoption > 0)
	{
		for(i=menu.curoption-1;i>=0;i--)
		{
			if (menu_gettype(i) != MNU_SEPERATOR) { canmove = 1; break; }
		}
	}
	
	if (canmove)
	{
		menu.twirly_yoff_vector = -1;
	}
	else
	{
		scrollfunc = (*menu.curmenu)[HD_SCROLLFUNC];
		if (scrollfunc)
			(*scrollfunc)(UP);
		else
		{
			menu.curoption = menu.last_option;
			while(menu_gettype(menu.curoption) == MNU_SEPERATOR) menu.curoption--;
		}
	}
}

// handle the menu. the menu is controlled using KeyDrv_KeyIsDown
// directly to SDLK keys. This keeps navigation from getting screwed
// up if you accidently mess up the keyboard mappings.
void Menu_HandleMenu(void)
{
static int twirly_move_timer=0;

	if (!menu.visible) return;
	Menu_HandleCount++;
	
	twirly_spin(&menu.curtwirlframe, &menu.twirltimer);
	
	// handle moving up and down
	if (menu.twirly_yoff_vector == 0)
	{
		twirly_move_timer = 999;
		if (KeyDrv_KeyIsDown(SDLK_DOWN))
		{
			if (menu.lastkeydir != KDOWN && !menu.cantmove)
			{
				move_down();
				menu.lastkeydir = KDOWN;
			}
		}
		else if (KeyDrv_KeyIsDown(SDLK_UP))
		{
			if (menu.lastkeydir != KUP && !menu.cantmove)
			{
				move_up();
				menu.lastkeydir = KUP;
			}
		}
		else if (KeyDrv_KeyIsDown(SDLK_RETURN))
		{
			if (menu.lastkeydir != KENTER)
			{
				Menu_SelectOption(menu.curoption);
				menu.lastkeydir = KENTER;
			}
		}
		else menu.lastkeydir = 255;
	}
	else if (twirly_move_timer >= 3)
	{
		twirly_move_timer = 0;
		// move the twirly until it reaches it's new option
		menu.twirly_yoff += menu.twirly_yoff_vector;
		if (abs(menu.twirly_yoff) >= TWIRLY_YOFF_MOVEAMOUNT)
		{
			menu.twirly_yoff = 0;
			menu.curoption += menu.twirly_yoff_vector;
			
			if (menu_gettype(menu.curoption) != MNU_SEPERATOR)
			{
				menu.twirly_yoff_vector = 0;
			}
		}
	}
	else twirly_move_timer++;
}


// draw an empty dialog box
// x1 and y1 are pixel positions,
// w and h are width and height of the box in tiles
void dialogbox(int x1, int y1, int w, int h)
{
int x,y,i,j;

	drawcharacter(x1, y1, 1);			// upper-left corner
	drawcharacter(x1+(w*8), y1, 3);		// upper-right corner
	// draw top border
	for(x=x1+8,i=0;i<w-1;i++)
	{
		drawcharacter(x, y1, 2);
		x += 8;
	}
	
	// draw sides and fill in
	y = (y1+8);
	for(j=0;j<h-2;j++)
	{
		for(x=x1,i=0;i<=w;i++)
		{
			if (i==0) drawcharacter(x, y, 4);
			else if (i==w) drawcharacter(x, y, 5);
			else drawcharacter(x, y, ' ');
			x+=8;
		}
		y+=8;
	}
	
	// draw bottom border
	for(x=x1,i=0;i<=w;i++)
	{
		if (i==0) drawcharacter(x, y, 6);
		else if (i==w) drawcharacter(x, y, 8);
		else drawcharacter(x, y, 7);
		x+=8;
	}
}

char static goto_epmenu(int numplayers)
{
	newgame_number_of_players = numplayers;
	Menu_InitMenu(&selectep_menu);
	return 0;
}

char static mmhandler_newgame(int episode)
{
	if (episode_available[episode])
	{
		endlevel(STARTING_NEW_GAME);
		newgame_episode = episode;
		return 1;
	}
	else
	{
		Menu_InitMenu(&cant_play_ep_menu);
		return 0;
	}
}

char static mmhandler_endgame(int dummy)
	{ endlevel(ENDING_GAME); return 1; }

char static mmhandler_restartlevel(int dummy)
{
int i;
	if (levelcontrol.play_custom_level)
	{
		Menu_InitMenu(&endgame_menu);
		return 0;
	}
	else
	{
		for(i=0;i<MAX_PLAYERS;i++)
			if (player[i].isPlaying) killplayer(i);
	}
	return 1;
}

char static mmhandler_quit(int dummy)
{
	if (!levelcontrol.gameinprogress || levelcontrol.gameover)
	{
		mmhandler_quit2(0);
		return 1;
	}
	else
	{
		Menu_InitMenu(quit_menu);
		return 0;
	}
}

char static mmhandler_quit2(int dummy)
{
	endlevel(QUITTING_PROGRAM);
	return 1;
}
	
char dismiss_menu(int dummy)		// all this does is make the menu go away
	{ return 1; }
