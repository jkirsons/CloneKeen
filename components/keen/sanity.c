// simple sanity check before starting up to make sure all files are there
// also determines which episodes are available

#include "keen.h"
#include "sanity.fdh"

#define	demo_file  "This is a recorded demo file."
#define tile_gfx   "Tile graphics file"
#define level      "Level file"
#define attrfile   "This file contains properties of tiles used in game levels."
#define gfxfile	   "Graphics file"

#define COMESWITHCLONE		(char *)(0)
#define COMESWITHKEEN		(char *)(1)

char *sanity[99] = {"strings.dat", "This file contains text used in the game.", COMESWITHCLONE,
					"ep1demo1.dat", demo_file, COMESWITHCLONE,
					"ep1demo2.dat", demo_file, COMESWITHCLONE,
					"ep1attr.dat", attrfile, COMESWITHCLONE,
					"demolvl.ck1", "The level used in the intro demo.", COMESWITHCLONE,
					NULL };
					  
char *sanity_ep[99] = {"ep?attr.dat", attrfile, COMESWITHCLONE,
					   "SOUNDS.CK?", "Sounds for the game.", COMESWITHKEEN,
					   "EGAHEAD.CK?", tile_gfx, COMESWITHKEEN,
					   "EGALATCH.CK?", tile_gfx, COMESWITHKEEN,
					   "EGASPRIT.CK?" ,"Sprite graphics file", COMESWITHKEEN,
					   "FINALE.CK?", "Contains gfx for the end sequence", COMESWITHKEEN,
					   "LEVEL01.CK?", level, COMESWITHKEEN,
					   "LEVEL02.CK?", level, COMESWITHKEEN,
					   "LEVEL03.CK?", level, COMESWITHKEEN,
					   "LEVEL04.CK?", level, COMESWITHKEEN,
					   "LEVEL05.CK?", level, COMESWITHKEEN,
					   "LEVEL06.CK?", level, COMESWITHKEEN,
					   "LEVEL07.CK?", level, COMESWITHKEEN,
					   "LEVEL08.CK?", level, COMESWITHKEEN,
					   "LEVEL09.CK?", level, COMESWITHKEEN,
					   "LEVEL10.CK?", level, COMESWITHKEEN,
					   "LEVEL11.CK?", level, COMESWITHKEEN,
					   "LEVEL12.CK?", level, COMESWITHKEEN,
					   "LEVEL13.CK?", level, COMESWITHKEEN,
					   "LEVEL14.CK?", level, COMESWITHKEEN,
					   "LEVEL15.CK?", level, COMESWITHKEEN,
					   "LEVEL16.CK?", level, COMESWITHKEEN,
					   "LEVEL80.CK?", level, COMESWITHKEEN,
					   "LEVEL81.CK?", level, COMESWITHKEEN,
					   "LEVEL90.CK?", level, COMESWITHKEEN,
						NULL };

const char *kNoSharewareEpisode =
{
	"You don't have the shareware episode!\n\n"
	"CloneKeen requires the shareware episode\n"
	"be installed in order to play. You can\n"
	"download it for free--a link is on\n"
	"CloneKeen's SourceForge page.\n"
	"\n"
	"Please unzip the shareware episode and\n"
	"place it into the \"data\" subdirectory,\n"
	"and then restart the game."
};

const char *kFromOriginal =
{
	"This is a data file from the original game.\n"
	"Please put a copy of the original game files\n"
	"into the 'data' subdirectory beneath your\n"
	"CloneKeen installation, and restart."
};

const char *kFromCloneKeen =
{
	"This file is part of CloneKeen and should\n"
	"have come with it. If you compiled the game\n"
	"from source, remember that you should\n"
	"copy your executable into the 'bin'\n"
	"directory before running it."
};


char sanity_check(void)
{
int bad;
int ep;
	lprintf("Sanity check...\n");
	
	// check sanity of files needed for the game, period
	bad = run_sanity((void *)sanity, 0);
	if (bad)
	{
		lprintf("You are missing some files needed for startup. See above for details.\n");
		return 1;
	}
	else lprintf("CloneKeen files appear ok.\n");
	
	// check whether each episode is present
	for(ep=1;ep<=3;ep++)
	{
		episode_available[ep] = (1 - run_sanity((void *)sanity_ep, ep));
		if (episode_available[ep])
		{
			if (ep==1) lprintf("  * You have shareware episode.\n");
				  else lprintf("  * You have episode %d.\n", ep);
		}
		else
		{
			if (ep==1)
			{
				platform_msgbox(kNoSharewareEpisode);
				return 1;
			}
			else
				lprintf("You DO NOT have episode %d.\n", ep);
		}
	}
	
	lprintf("\n");
	return 0;
}

// check all the files in the provided list to see if they exist,
// replacing any question marks in their names with "episode".
// display errors for missing files, and return 1 if any are missing.
char run_sanity(char *(*list)[], uchar episode)
{
int i,j;
char fname[1024], fullfname[1024];
char whatitis[100];
char wheretogetit;
char missing = 0;

	episode = '0' + episode;
	
	i = 0;
	while ((*list)[i])
	{
		strcpy(fname, (*list)[i]);
		for(j=0;j<fname[j];j++) { if (fname[j]=='?') fname[j]=episode; }
		
		wheretogetit = (char)(*list)[i+2];
		if (wheretogetit)
			sprintf(fullfname, "data/%s", fname);
		else
			strcpy(fullfname, fname);
		
		if (!file_exists(fullfname))
		{
			strcpy(whatitis, (*list)[i+1]);
			
			if (!wheretogetit || episode=='1')
			{
				char bigerror[5000];
				char *getFromWhere;
				
				if (wheretogetit == COMESWITHKEEN || 1)
					getFromWhere = kFromOriginal;
				else
					getFromWhere = kFromCloneKeen;
				
				lprintf("\nsanity check: file missing: '%s'\n", fullfname);
				
				sprintf(bigerror, "[[ MISSING FILE: '%s' ]]\n%s\n\n%s",
									fullfname, whatitis, getFromWhere);
				platform_msgbox(bigerror);
			}
			
			missing = 1;
			break;
		}
		i += 3;
	}
		
	return missing;
}

char file_exists(char *fname)
{
FILE *fp;
	fp = fileopen(fname, "rb");
	if (!fp) return 0;
	__fclose(fp); return 1;
}
