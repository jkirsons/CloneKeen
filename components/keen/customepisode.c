
#include "keen.h"
#include "customepisode.fdh"
#include "vgatiles.fdh"

#define EPCONFIG_NAME			"flags.ini"

// load the "flags.ini" configuration file.
char LoadFlagsIni(void)
{
char iname[MAXPATHLEN];
int i;

	// try to find an ini file describing the episode...
	sprintf(iname, "%s%s", levelcontrol.custom_episode_path, EPCONFIG_NAME);
	lprintf("loading %s\n", iname);
	
	if (Ini_GetNumericKeyDef(iname, "DoorsOpenUpwards", 0))
		levelcontrol.cepvars.DoorOpenDir = UP;
	else
		levelcontrol.cepvars.DoorOpenDir = DOWN;
	
	newgame_number_of_players = Ini_GetNumericKeyDef(iname, "ForcePlayers", newgame_number_of_players);
	levelcontrol.cepvars.VortDieDoFade = Ini_GetNumericKeyDef(iname, "VortDieDoFade", 1);
	levelcontrol.cepvars.pShotSpeed = Ini_GetNumericKeyDef(iname, "pShotSpeed", 0);
	
	for(i=0;i<numplayers;i++)
	{
		player[i].inventory.HasPogo = Ini_GetNumericKeyDef(iname, "StartWithPogo", player[i].inventory.HasPogo);
		player[i].inventory.charges = Ini_GetNumericKeyDef(iname, "StartWithCharges", player[i].inventory.charges);
	}
	
	if (Ini_GetNumericKeyDef(iname, "VGASprites", 0)) LoadVGASprites();
	if (Ini_GetNumericKeyDef(iname, "VGATiles", 0)) LoadVGATiles();
	return 0;
}

// determine which episode the custom episode at "path" is designed for.
// i.e., which episode should it "behave" like.
char Custom_Get_Episode(char *path)
{
char iname[MAXPATHLEN];
int ep;
	sprintf(iname, "%s%s", path, EPCONFIG_NAME);
	ep = Ini_GetNumericKey(iname, "ForceEpisode");
	
	if (ep==-1)
		ep = GuessWhichEpisode(path);
	return ep;
}

// analyze a custom episode directory and try to guess which episode
// is it a mod of based on the most predominant file extension
char static GuessWhichEpisode(char *basepath)
{
int i,nfiles;
int counts[3];
char *fname, *ext;
char path[MAXPATHLEN];
int ep;

	lprintf("Guessing which episode this custom episode is based on...\n");
	counts[0] = counts[1] = counts[2] = 0;
	
	sprintf(path, "%sdata/", basepath);
	nfiles = LoadDirectoryListing(path);
	for(i=0;i<nfiles;i++)
	{
		fname = GetFileAtIndex(i);
		ext = strrchr(fname, '.');
		if (ext && strlen(ext)==4 && toupper(ext[1])=='C' && toupper(ext[2])=='K')
		{
			if (ext[3] >= '1' && ext[3] <= '3')
			{
				counts[ext[3]-'1']++;
			}
		}
	}
	lprintf("GuessWhichEpisode: counts: %d %d %d\n", counts[0], counts[1], counts[2]);
	if (counts[2] > counts[0] && counts[2] > counts[1]) return 3;
	if (counts[1] > counts[0] && counts[1] > counts[2]) return 2;
	return 1;
}
