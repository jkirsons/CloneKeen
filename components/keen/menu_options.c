// Saved options

#include "keen.h"
#include "menu_options.fdh"
#include "menu.h"

int saved_options[NUM_OPTIONS];
extern uchar *keysetup_menu1[MAXM];
extern uchar *keysetup_menu2[MAXM];
char tempbuf[80];

uchar *option_menu[MAXM] = {(6*8)+2,2,  8, options_savestate, SaveOptions, NULL, NULL,
					"Render Mode", MNU_DYNAMICTEXT, SetRenderMode, 0,
					"Render FPS", MNU_DYNAMICTEXT, SetRenderFPS, 0,
					"P1 Color", MNU_DYNAMICTEXT, SetPColor, OPT_P1COLOR,
					"P2 Color", MNU_DYNAMICTEXT, SetPColor, OPT_P2COLOR,
					"Input P2...", MNU_SWITCHMENU, keysetup_menu2, 0,
					"Input P1...", MNU_SWITCHMENU, keysetup_menu1, 0,
					"  Fullscreen", MNU_DYNAMICTEXT, OptionSetter, OPT_FULLSCREEN,
					//"", MNU_SEPERATOR, NULL, 0,
					//"--Gameplay Options---", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,
					"  2-Button Firing", MNU_DYNAMICTEXT, OptionSetter, OPT_2BUTTONFIRING,
                                        "  Fully Automatic Raygun", MNU_DYNAMICTEXT, OptionSetter, OPT_FULLYAUTOMATIC,
					"  Super Pogo", MNU_DYNAMICTEXT, OptionSetter, OPT_SUPERPOGO,
					"  Enable Cheats", MNU_DYNAMICTEXT, OptionSetter, OPT_CHEATS,
					"  Show Bonus Points", MNU_DYNAMICTEXT, OptionSetter, OPT_RISEBONUS,
					"  Show OSD", MNU_DYNAMICTEXT, OptionSetter, OPT_SHOWOSD,
					"  Show Score in OSD", MNU_DYNAMICTEXT, OptionSetter, OPT_SHOWSCORE,
					"  Show Cards in OSD", MNU_DYNAMICTEXT, OptionSetter, OPT_SHOWCARDS,
					"  Doors Block Raygun", MNU_DYNAMICTEXT, OptionSetter, OPT_DOORSBLOCKRAY,
					"  Quiet in Demo", MNU_DYNAMICTEXT, OptionSetter, OPT_SOUNDOFFINDEMO,
					"", MNU_SEPERATOR, NULL, 0,
					"  Increase Difficulty", MNU_DYNAMICTEXT, OptionSetter, OPT_HARD,
					"  Must Kill All Yorps", MNU_DYNAMICTEXT, OptionSetter, OPT_MUSTKILLYORPS,
					"  Give Yorps Shields", MNU_DYNAMICTEXT, OptionSetter, OPT_YORPFORCEFIELDS,
					"", MNU_SEPERATOR, NULL, 0,
					"Accept", MNU_CALLFP, opthandler_accept, 0,
					"Cancel", MNU_CALLFP, opthandler_cancel, 0,
					NULL, NULL, NULL, 0
					};
					
					
char *SetRenderFPS(char *MenuText, char set, char param)
{
	if (!set)
	{
		if (options[OPT_RENDERRATE] < 10 || options[OPT_RENDERRATE] > 120)
			options[OPT_RENDERRATE] = 60;
			
		sprintf(tempbuf, "%s   %02d", MenuText, options[OPT_RENDERRATE]);
		return tempbuf;
	}
	else
	{
		switch(options[OPT_RENDERRATE])
		{
			case 60: options[OPT_RENDERRATE] = 50; break;
			case 50: options[OPT_RENDERRATE] = 40; break;
			case 40: options[OPT_RENDERRATE] = 25; break;
			case 25: options[OPT_RENDERRATE] = 20; break;
			case 20: options[OPT_RENDERRATE] = 10; break;
			default: options[OPT_RENDERRATE] = 60; break;
		}
		return NULL;
	}
}

char *SetRenderMode(char *MenuText, char set, char param)
{
	if (!set)
	{
		sprintf(tempbuf, "%s  %s%s", MenuText, options[OPT_ZOOMONRESTART]?"640x480":"320x240", (options[OPT_ZOOM]==options[OPT_ZOOMONRESTART])?"":" [restart]");
		return tempbuf;
	}
	else
	{
		options[OPT_ZOOMONRESTART] ^= 1;
		return NULL;
	}
}

char *SetPColor(char *MenuText, char set, char param)
{
	if (!set)
	{
		char *colour;
		switch(options[param])
		{
			case PCOLOR_STD: colour = "STANDARD"; break;
			case PCOLOR_YELGREEN: colour = "YEL/GREEN"; break;
			case PCOLOR_BLUEGREEN: colour = "BLUE/GREEN"; break;
			case PCOLOR_RED: colour = "RED"; break;
			default: colour = "UNKNOWN"; break;
		}
		sprintf(tempbuf, "%s     %s", MenuText, colour);
		return tempbuf;
	}
	else
	{
		options[param]++;
		if (options[param] >= PCOLOR_LAST) options[param] = 0;
		setplayercolors();
		return NULL;
	}
}


char static opthandler_cancel(int arg)
{
int i;
	// options screen was "canceled", restore all options to original values
	for(i=0;i<NUM_OPTIONS;i++)
	{
		options[i] = saved_options[i];
	}
	Menu_ToMain();
	return 0;
}

char static opthandler_accept(int arg)
{
	SaveOptions();
	Menu_ToMain();
	return 0;
}

void static options_savestate(void)
{
int i;
	for(i=0;i<NUM_OPTIONS;i++) saved_options[i] = options[i];
}


char *OptionSetter(char *MenuText, char set, uchar opt_index)
{
	if (!set)
	{	// fetching value
		strcpy(tempbuf, MenuText);
		tempbuf[0] = options[opt_index] ? OPTION_ENABLED_CHAR:OPTION_DISABLED_CHAR;
		return tempbuf;
	}
	else	// setting value
	{
		options[opt_index] ^= 1;
		return NULL;	// this is ignored anyways
	}
}
