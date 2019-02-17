#include "keen.h"
#include "menu_savegames.fdh"
#include "menu.h"

// Menu for save and load games
uchar tempbuf[80];

uchar *loadgame_menu[MAXM] = {(7*8)-3,70,  0, loadsavegamenames, NULL, NULL, NULL,
					"1", MNU_DYNAMICTEXT, loadgame_getgamename, 1,
					"2", MNU_DYNAMICTEXT, loadgame_getgamename, 2,
					"3", MNU_DYNAMICTEXT, loadgame_getgamename, 3,
					"4", MNU_DYNAMICTEXT, loadgame_getgamename, 4,
					"5", MNU_DYNAMICTEXT, loadgame_getgamename, 5,
					"6", MNU_DYNAMICTEXT, loadgame_getgamename, 6,
					"7", MNU_DYNAMICTEXT, loadgame_getgamename, 7,
					"8", MNU_DYNAMICTEXT, loadgame_getgamename, 8,
					"9", MNU_DYNAMICTEXT, loadgame_getgamename, 9,
					"", MNU_SEPERATOR, NULL, 0,
					"Cancel", MNU_CALLFP, dismiss_menu, 0,
					};
uchar *savegame_menu[MAXM] = {(7*8)-3,70,  0, loadsavegamenames, NULL, NULL, NULL,
					"1", MNU_DYNAMICTEXT, savegame_getgamename, 1,
					"2", MNU_DYNAMICTEXT, savegame_getgamename, 2,
					"3", MNU_DYNAMICTEXT, savegame_getgamename, 3,
					"4", MNU_DYNAMICTEXT, savegame_getgamename, 4,
					"5", MNU_DYNAMICTEXT, savegame_getgamename, 5,
					"6", MNU_DYNAMICTEXT, savegame_getgamename, 6,
					"7", MNU_DYNAMICTEXT, savegame_getgamename, 7,
					"8", MNU_DYNAMICTEXT, savegame_getgamename, 8,
					"9", MNU_DYNAMICTEXT, savegame_getgamename, 9,
					"", MNU_SEPERATOR, NULL, 0,
					"Cancel", MNU_CALLFP, dismiss_menu, 0,
					};
					
uchar *savegame_failure[MAXM] = {4,94,  255, NULL,
					"Failure-- Unable to save the game.", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"Ok", MNU_CALLFP, dismiss_menu, 0,
					NULL, NULL, NULL, 0
					};
					
					
struct
{
	uchar level;
	uchar episode;
	uchar lives;
	uchar numplayers;
	char iscustom;
	char exists;
} savegames[10];

#define JUSTSAVED_TIME		7
#define XXX_TIME			80
int justsaved=-1, justsavedtimer, justsaved_stars, xxxtimer;

void loadsavegamenames(void)
{
FILE *fp;
int i;
char fname[40];
	justsaved = -1;
	for(i=1;i<10;i++)
	{
		sprintf(fname, "savegame%d.dat", i);
		savegames[i].exists = 0;
		if (IsValidSaveGame(fname))
		{
			fp = fileopen(fname, "rb");
			if (fp)
			{
				savegame_readheader(fp, &savegames[i].episode, \
									&savegames[i].level, &savegames[i].lives,\
									&savegames[i].numplayers, &savegames[i].iscustom);
				if (savegames[i].lives > 9) savegames[i].lives = 9;
				savegames[i].exists = 1;
				__fclose(fp);
			}
		}
	}
}

char static *getsgstr(int slot)
{
#define BLANKGAME	"[                   ]"
#define XXXGAME		"[xxxxxxxxxxxxxxxxxxx]"
int x,i;
	
	if (slot==1)
	{
		if (!options[OPT_ZOOM])
		{
			x = (WINDOW_WIDTH - (23*8)) / 2;
			font_draw("Use DEL to delete games", x, 222, drawcharacter_clear);
		}
		else
		{
			options[OPT_ZOOM] = 0;
			x = (WINDOW_2X_WIDTH - (23*8)) / 2;
			font_draw("Use DEL to delete games", x, 390, drawcharacter_clear_erasebg);
			options[OPT_ZOOM] = 1;
		}
	}
	
	if (savegames[slot].exists)
	{
		if (savegames[slot].iscustom)
		{
			sprintf(tempbuf, "[USERM - %d lives  %cp]", savegames[slot].lives, savegames[slot].numplayers+'0');
		}
		else
		{
			if (savegames[slot].level==WORLD_MAP)
				sprintf(tempbuf, "[E%d WM - %d lives  %cp]", savegames[slot].episode, savegames[slot].lives, savegames[slot].numplayers+'0');
			else
				sprintf(tempbuf, "[E%dL%02d - %d lives  %cp]", savegames[slot].episode, savegames[slot].level, savegames[slot].lives, savegames[slot].numplayers+'0');
		}
		
		// allow deletion of save games via DEL
		if (keytable[KDEL])
		{
			DeleteSaveGame(menu.curoption+1);
			savegames[menu.curoption+1].exists = 0;
		}
	}
	else
		strcpy(tempbuf, BLANKGAME);
	
	if (slot==justsaved)
	{
		while(Menu_HandleCount > 0)
		{
			if (xxxtimer)
			{
				xxxtimer--;
				strcpy(tempbuf, XXXGAME);
			}
			else
			{
				if (!justsavedtimer)
				{
					justsavedtimer = JUSTSAVED_TIME;
					justsaved_stars++;
				}
				else justsavedtimer--;
				
				x = strlen(BLANKGAME) / 2;
				tempbuf[x] = '*';
				for(i=0;i<justsaved_stars;i++)
				{
					if (x-i==0)
					{
						justsaved = -1;
						return tempbuf;
					}
					tempbuf[x+i] = '*';
					tempbuf[x-i] = '*';
				}
			}
			
			Menu_HandleCount--;
		}
	}
	
	return tempbuf;
}

char *loadgame_getgamename(char *MenuText, char set, uchar i)
{
	justsaved = -1;
	if (!set)
	{
		return getsgstr(i);
	}
	else
	{
		if (savegames[i].exists)
		{
			levelcontrol.gametoload = i;
			endlevel(LOADING_GAME);
			Menu_SetVisibility(0);
		}
	}
	return 0;
}

char *savegame_getgamename(char *MenuText, char set, uchar i)
{
	if (!set)
	{
		return getsgstr(i);
	}
	else
	{
		if (game_save(i))
		{
			Menu_InitMenu(&savegame_failure);
			return 0;
		}
		sound_play(SOUND_USE_KEY, PLAY_NOW);
		loadsavegamenames();
		
		justsaved = i;
		justsavedtimer = JUSTSAVED_TIME;
		xxxtimer = XXX_TIME;
		justsaved_stars = 0;
		Menu_HandleCount = 0;
		return 0;
	}
}
