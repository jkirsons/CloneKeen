#include "../keen.h"
#include "menu_editor.fdh"
#include "../menu.h"

#include "editor.fdh"
extern uchar *edverifyclear_menu[MAXM];
extern uchar *editor_misc_menu[MAXM];
char static me_resetmap(int arg);
char static me_newmap(int arg);
char static me_nextep(int arg);
char static me_save(int arg);
char static me_saveas(int arg);
char static me_exit_ed(int arg);
char static me_resizemap(int arg);
char static me_addborder(int arg);
char static me_timelimit(int arg);
void cmap_enter(void);
void editor_loadmap_exit(void);
void cmap_scroll(char dir);
char *getcmapname(char *MenuText, char set, uchar i);

char tempbuf[80];

uchar *editor_misc_menu[MAXM] = {64,72,  0, NULL, NULL, NULL, NULL,
					"Set Time Limit", MNU_CALLFP, me_timelimit, 0,
					"  Game Over on Time Out", MNU_DYNAMICTEXT, SetGameOverOnTimeOut, 0,
					"", MNU_DYNAMICTEXT, SetForcedOption, OPT_HARD,
					"", MNU_DYNAMICTEXT, SetForcedOption, OPT_MUSTKILLYORPS,
					"", MNU_DYNAMICTEXT, SetForcedOption, OPT_YORPFORCEFIELDS,
					"", MNU_DYNAMICTEXT, SetForcedOption, OPT_FULLYAUTOMATIC,
					"", MNU_DYNAMICTEXT, SetForcedOption, OPT_SUPERPOGO,
					"", MNU_DYNAMICTEXT, SetForcedOption, OPT_DOORSBLOCKRAY,
					"", MNU_SEPERATOR, NULL, 0,
					"Add Border", MNU_CALLFP, me_addborder, 0,
					"[M] Set Map Size", MNU_CALLFP, me_resizemap, 0,
					"  [G] Show Object Ghosting", MNU_DYNAMICTEXT, ToggleGhosting, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"<< Back", MNU_SWITCHMENU, editor_menu, 0,
					NULL, NULL, NULL, 0
					};

uchar *editor_menu[MAXM] = {75+8,61,  0, NULL, NULL, NULL, NULL,
					"[z]  Run/Stop", MNU_CALLFP, me_togglerunstop, 0,
					"[x]  Reset", MNU_CALLFP, me_resetmap, 0,
					"     Clear Map", MNU_SWITCHMENU, edverifyclear_menu, 0,
					"[e]  Change Episode", MNU_CALLFP, me_nextep, 0,
					"[F1] Save", MNU_CALLFP, me_save, 0,
					"[F2] Save As...", MNU_CALLFP, me_saveas, 0,
					"Misc", MNU_SWITCHMENU, editor_misc_menu, 0,
					"Forced Game Options", MNU_SWITCHMENU, option_menu, 0,
					"Load Map", MNU_SWITCHMENU, edloadmap_menu, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"Exit Editor", MNU_CALLFP, me_exit_ed, 0,
					"Exit Program", MNU_SWITCHMENU, quit_menu, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"q - Set Start Pos", MNU_SEPERATOR, NULL, 0,
					"b, ALT+b - Block/Pat Fill", MNU_SEPERATOR, NULL, 0,
					"SHIFT - Pickup Tile", MNU_SEPERATOR, NULL, 0,
					"W,A,S,D - Move Selector", MNU_SEPERATOR, NULL, 0,
					NULL, NULL, NULL, 0
					};
					
uchar *edverifyclear_menu[MAXM] = {92,100,0,NULL,NULL,NULL,NULL,
					"Erase Map!", MNU_CALLFP, me_newmap, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"Do nothing.", MNU_SWITCHMENU, editor_menu, 0,
					NULL, NULL, NULL, 0
					};
					
// this menu steals a lot of stuff from menu_custommap.c
uchar *edloadmap_menu[MAXM] = {(7*9)-3,72,  0, cmap_enter, editor_loadmap_exit, cmap_scroll, NULL,
					"<New Map>", MNU_CALLFP, me_newmap, 0,
					"0", MNU_DYNAMICTEXT, getcmapname, 100,
					"1", MNU_DYNAMICTEXT, getcmapname, 101,
					"2", MNU_DYNAMICTEXT, getcmapname, 102,
					"3", MNU_DYNAMICTEXT, getcmapname, 103,
					"4", MNU_DYNAMICTEXT, getcmapname, 104,
					"5", MNU_DYNAMICTEXT, getcmapname, 105,
					"6", MNU_DYNAMICTEXT, getcmapname, 106,
					"7", MNU_DYNAMICTEXT, getcmapname, 107,
					"8", MNU_DYNAMICTEXT, getcmapname, 108,
					"9", MNU_DYNAMICTEXT, getcmapname, 109,
					"10", MNU_DYNAMICTEXT, getcmapname, 110,
					};
uchar *edbadlevel_menu[MAXM] = {(4*8),90,  255, NULL, NULL, NULL, NULL,
					"I don't know how to open", MNU_SEPERATOR, NULL, 0,
					"that file!", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"Ok", MNU_SWITCHMENU, edloadmap_menu, 0,
					};
					

char me_togglerunstop(int arg)
{
	if (levelcontrol.level_done==LEVEL_NOT_DONE)
	{
		setrunstop(editor_gameisrunning() ^ 1);
	}
	return 1;
}

char static me_resetmap(int arg) { editor_resetmap(); return 1; }
char static me_newmap(int arg)
{
	strcpy(levelcontrol.custom_level_name, "");
	editor_enter();
	return 1;
}
char static me_addborder(int arg) { editor_addborder(); return 1; }
char static me_nextep(int arg) { editor_nextep(); return 1; }
char static me_save(int arg) { editor_save(); return 1; }
char static me_saveas(int arg) { editor_saveas(); return 1; }
char static me_exit_ed(int arg) { editor_exit(); return 1; }
char static me_resizemap(int arg) { editor_resizemap(); return 1; }
char static me_timelimit(int arg) { editor_settimelimit(); return 1; }


/*char notdone(int arg)
{
	Console_Msg("Function not implemented at this time.");
	return 1;
}
*/

char static *SetGameOverOnTimeOut(char *MenuText, char set, uchar crap)
{
	if (!set)
	{
		strcpy(tempbuf, MenuText);
		tempbuf[0] = map.GameOverOnTimeOut ? OPTION_ENABLED_CHAR:OPTION_DISABLED_CHAR;
		return tempbuf;
	}
	else
	{
		map.GameOverOnTimeOut ^= 1;
	}
}

char static *ToggleGhosting(char *MenuText, char set, uchar crap)
{
	if (!set)
	{
		strcpy(tempbuf, MenuText);
		tempbuf[0] = options[OPT_GHOSTING] ? OPTION_ENABLED_CHAR:OPTION_DISABLED_CHAR;
		return tempbuf;
	}
	else
	{
		editor_set_do_ghosting(options[OPT_GHOSTING] ^ 1);
	}
}

char static *SetForcedOption(char *MenuText, char set, uchar option)
{
char *name, *on, *off;
	if (!set)
	{
		on = "ON"; off = "OFF";
		switch(option)
		{
			case OPT_HARD:
				name = "Skill: "; on = "Hard"; off = "Normal";
				break;
			case OPT_MUSTKILLYORPS:
				name = "Must Kill Yorps: ";
				break;
			case OPT_YORPFORCEFIELDS:
				name = "Yorp Shielding: ";
				break;
			case OPT_FULLYAUTOMATIC:
				name = "Autofire: ";
				break;
			case OPT_SUPERPOGO:
				name = "SuperPogo: ";
				break;
			case OPT_DOORSBLOCKRAY:
				name = "Doors Block Shots: ";
				break;
			
			default:
				return "[[ERROR]]";
		}
	
		strcpy(tempbuf, name);
		switch(map.forced_options[option])
		{
			case OPTION_NOT_FORCED: strcat(tempbuf, "Default"); break;
			case OPTION_FORCE_ENABLE: strcat(tempbuf, on); break;
			case OPTION_FORCE_DISABLE: strcat(tempbuf, off); break;
			default: strcat(tempbuf, "[Error]"); break;
		}
		return tempbuf;
	}
	else
	{
		switch(map.forced_options[option])
		{
			case OPTION_NOT_FORCED: map.forced_options[option] = OPTION_FORCE_ENABLE; break;
			case OPTION_FORCE_ENABLE: map.forced_options[option] = OPTION_FORCE_DISABLE; break;
			default: map.forced_options[option] = OPTION_NOT_FORCED; break;
		}
	}
}
