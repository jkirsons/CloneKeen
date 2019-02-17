#include "keen.h"
#include "menu_custommap.fdh"
#include "menu.h"
// Menu for loading custom maps/episodes
// Parts of this are also used by the editor's "load map" menu.

char tempbuf[80];

#define CM_NUM_VIS_ITEMS			11
uchar *custommap_menu[MAXM] = {(7*9)-3,72,  0, cmap_enter, cmap_exit, cmap_scroll, NULL,
					"0", MNU_DYNAMICTEXT, getcmapname, 0,
					"1", MNU_DYNAMICTEXT, getcmapname, 1,
					"2", MNU_DYNAMICTEXT, getcmapname, 2,
					"3", MNU_DYNAMICTEXT, getcmapname, 3,
					"4", MNU_DYNAMICTEXT, getcmapname, 4,
					"5", MNU_DYNAMICTEXT, getcmapname, 5,
					"6", MNU_DYNAMICTEXT, getcmapname, 6,
					"7", MNU_DYNAMICTEXT, getcmapname, 7,
					"8", MNU_DYNAMICTEXT, getcmapname, 8,
					"9", MNU_DYNAMICTEXT, getcmapname, 9,
					"10", MNU_DYNAMICTEXT, getcmapname, 10,
					};

uchar *badlevel_menu[MAXM] = {(4*8),90,  255, NULL, NULL, NULL, NULL,
					"I don't know how to open", MNU_SEPERATOR, NULL, 0,
					"this file!", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"Ok", MNU_SWITCHMENU, custommap_menu, 0,
					};
				
int nFiles;
int cm_scrolloffset = 0;
char cm_selected_something = 0;

void cmap_enter(void)
{
	cm_scrolloffset = 0;
	cm_selected_something = 0;
	nFiles = LoadDirectoryListing(CUSTOM_DIR);
}

void cmap_exit(void)
{
	FreeDirectoryListing();
	// when hit ESC, back out to episode menu, don't hide menu
	if (!cm_selected_something)
	{
		Menu_InitMenu(selectep_menu);
	}
}

void editor_loadmap_exit(void)
{
	FreeDirectoryListing();
	// when hit ESC, back out to main menu, don't hide menu
	if (!cm_selected_something)
	{
		Menu_InitMenu(main_menu);
	}
}


void cmap_scroll(char dir)
{
	if (dir==UP)
	{
		if (cm_scrolloffset > 0)
			cm_scrolloffset--;
		else
		{
			menu.curoption = menu.last_option;
			if (nFiles > CM_NUM_VIS_ITEMS)
				cm_scrolloffset = (nFiles - CM_NUM_VIS_ITEMS);
			else
				cm_scrolloffset = 0;
		}
	}
	else
	{
		if (cm_scrolloffset + CM_NUM_VIS_ITEMS < nFiles)
			cm_scrolloffset++;
		else
		{
			menu.curoption = 0;
			cm_scrolloffset = 0;
		}
	}
}


char *getcmapname(char *MenuText, char set, uchar i)
{
char *fname;
char *ext;
char valid;
char edloadmap=0;

	// find out if we're being called from menu_editor.c (edloadmap_menu)
	if (i >= 100)
	{
		i -= 100;
		edloadmap = 1;
	}
	
	fname = GetFileAtIndex(cm_scrolloffset + i);
	if (!set)
	{
		if (fname)
		{
			if (GetIsDirectory(cm_scrolloffset + i))
			{
				sprintf(tempbuf, "[%s]", fname);
				return tempbuf;
			}
			else
				return fname;
		}
		else
		{
			strcpy(tempbuf, "[]");
			return tempbuf;
		}
	}
	else
	{
		if (fname)
		{
			cm_selected_something = 1;
			// detect episode from extension
			valid = 0;
			ext = strrchr(fname, '.');
			if (ext)
			{
				if (toupper(ext[1])=='C' && toupper(ext[2])=='K')
				{
					if (ext[3] >= '1' && ext[3] <= '3')
					{
						newgame_episode = (ext[3] - '0');
						if (!episode_available[newgame_episode])
						{
							Menu_InitMenu(cant_play_usermap_menu);
							return NULL;
						}
						else
						{
						  valid = 1;
						}
					}
				}
			}
			else if (!edloadmap)
			{			
				// user selected a custom episode directory
				strcpy(custom_path, CUSTOM_DIR);
				strcat(custom_path, fname);
				strcat(custom_path, "/");
				endlevel(STARTING_CUSTOM_EPISODE);
				return NULL;
			}
			
			if (valid)
			{
				strcpy(levelcontrol.custom_level_name, CUSTOM_DIR);
				strcat(levelcontrol.custom_level_name, fname);
				if (!edloadmap)
					endlevel(STARTING_CUSTOM_LEVEL);
				else
					editor_enter();
			}
			else
			{
				if (!edloadmap)
					Menu_InitMenu(&badlevel_menu);
				else
					Menu_InitMenu(&edbadlevel_menu);
			}
		}
		return NULL;
	}
}
