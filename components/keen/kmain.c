/*
  MAIN.C
  This is CloneKeen's main source file.

  The CloneKeen source code may be freely distributed and
  modified as per the GPL, but please give credit to
  the original author, Caitlin Shaw.

  "Enjoy the Code"
         -Caitlin
*/

#include "keen.h"
#include "kmain.fdh"
 
#include "unistd.h"

char quitapp = 0;


// for the "real" main() function, see your platform specific
// source file under "platform" directory.
int KeenMain(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Delay(100);
    spi_lcd_clear();
    SDL_Delay(200);
    SDL_InitSD();
	//chdir("/sd/data/keen");

	memset(&levelcontrol, 0, sizeof(levelcontrol));
	memset(&playerbaseframes, 0, sizeof(playerbaseframes));
	levelcontrol.gamemode = GM_NORMAL;
	framebyframe = 0;
	
	last_editor = editor = 0;
	
	lprintf_init();
	
	banner(); lprintf("\n");
	// ensure all files needed for startup are present
	if (sanity_check()) return 1;
	
	LoadOptions();
	options[OPT_ZOOM] = options[OPT_ZOOMONRESTART];
  
	//getoption(OPT_HARD) = 1;
	//options[OPT_FULLSCREEN] = 1;
	
	if (Game_Startup())
	{
		if (!crashflag) crash("Game cannot start up.\n");
		goto displayCrash;
	}
	
	if (Load_Episode(1))
	{
		if (!crashflag) crash("Cannot load episode files.\n");
		goto displayCrash;
	}
  
	CreateTitle();
	Menu_ToMain();
	
	demo.current_demo = 1;
	memset(&overlay, 0, sizeof(overlay));
	
	lprintf("Game starting...\n");
	//if (intro()) goto exitt;
	
	EndGame();			// set no level active, so we go to demo on startup
	//levelcontrol.gamemode = GM_DEMORECORD;
	//StartGame(3,"",1);
	//levelcontrol.curlevel = 17;
	
	while(!crashflag && !quitapp)
	{
		if (RunNextLevel()) goto displayCrash;
	}
	
	if (!crashflag)
	{
		lprintf("Exiting normally.\n");
		banner();
		lprintf("\nThanks for playing!\n\n");
	}
	else
	{
		lprintf("\n> Egads-- something went wrong!\n");
	}
	
displayCrash: ;
	Game_Shutdown();
	
	if (crashflag && strstr(crash_reason, "Got quit event")==NULL)
	{
		char bigerror[20000];
		
		lprintf("game crash alert\n");
		lprintf(">Crash Details: %s\n", crash_reason);
		DumpOptions();
		
		sprintf(bigerror, "CloneKeen has exited abnormally!\n"
						  "Please see ck.log for details.\n"
						  "\n"
						  "\"%s\".", crash_reason);
		platform_msgbox(bigerror);
	}
	
	lprintf_close();
	return 0;
}

//char ft=1;
char RunNextLevel(void)
{
int result;
	lprintf("RunNextLevel...\n");
	
	///
	//levelcontrol.gameinprogress=1;
	//levelcontrol.curlevel=8;
	//levelcontrol.episode=1;
	//Menu_SetVisibility(0);
	
	if (!cinematic_is_playing() && !levelcontrol.gametoload && !editor)
	{
		// play demos when no real game running
		if (!levelcontrol.gameinprogress)
		{
			lprintf("RunNextLevel(): since gameinprogress=0, i'm gonna play a demo\n");
			if (start_demo(demo.current_demo))
			{	// reached last demo, loop back around
				start_demo(1);
				demo.current_demo = 2;
			}
			else demo.current_demo++;
		}
		else
		{		
			// reset variables and stuff to play a new level
			initgame();
			
			// in high-difficulity mode switch levels 5 & 9 so
			// you can't get the pogo stick until you make it
			// to the dark side of mars.
			if (levelcontrol.episode==1 && getoption(OPT_HARD))
			{
				if (levelcontrol.curlevel==5) levelcontrol.curlevel = 9;
				else if (levelcontrol.curlevel==9) levelcontrol.curlevel = 5;
			}
		}
		
		// load the next level into memory
		if (!levelcontrol.play_custom_level)
		{
			if (loadlevel(levelcontrol.curlevel))
			{
				crash("Error loading level #%d", levelcontrol.curlevel);
				return 1;
			}
		}
		else
		{
			//for(i=0;i<numplayers;i++) player[i].inventory.HasPogo = 1;
			levelcontrol.curlevel = 1;
			if (loadmap(levelcontrol.custom_level_name))
			{
				crash("Unable to open custom level '%s'\n", levelcontrol.custom_level_name);
				return 1;
			}
		}
	}
	
	// run the level
	///if (ft){result=ENTERING_EDITOR;ft=0;} else
	result = gameloop();
	lprintf("RunNextLevel: gameloop() returned result: %d\n", result);
	
	// take away any keycards they collected, don't let them carry them
	// to the next level
	take_all_keycards();
	
	switch(result)
	{
		case FOUND_SECRET_LEVEL:
			levelcontrol.bonuswarp = 1;
		case WON_LEVEL:
			if (levelcontrol.play_custom_level)
			{
				EndGame();
			}
			else
			{
				levelcontrol.levels_completed[levelcontrol.curlevel] = 1;
				levelcontrol.curlevel = WORLD_MAP;
				if (game_is_won()) 
				{
					// you won the game, dude!
					lprintf("> you won the game, dude!\n");
					endsequence_start();
				}
			}
			break;
			
		case LOST_LEVEL:
			if (game_is_over())
			{
				EndGame();
			}
			else
			{
				player[whodied].inventory.lives--;
				if (!levelcontrol.play_custom_level)
				{
					levelcontrol.curlevel = WORLD_MAP;
					keensleft();
				}
			}
			break;
		
		case HIT_TANTALUS_SWITCH:
			seq_tantalus_start();
			break;
		
		case ENTERING_EDITOR:
			if (!editor)
			{
				editor = 1;
				VidDrv_SetFullscreen(options[OPT_FULLSCREEN]);	// enlarge window
			}
			editor_init();
			break;
		case STARTING_NEW_GAME:
			StartGame(newgame_episode, "", newgame_number_of_players);
			break;
		
		case STARTING_CUSTOM_LEVEL:
			StartGame(newgame_episode, "", newgame_number_of_players);
			// custom_episode_name was already set in menu_custommap.c //
			lprintf("Entering custom level '%s'\n", levelcontrol.custom_level_name);
			levelcontrol.play_custom_level = 1;
			break;
		
		case STARTING_CUSTOM_EPISODE:
			newgame_episode = Custom_Get_Episode(custom_path);
			StartGame(newgame_episode, custom_path, newgame_number_of_players);
			break;
		
		case LEAVING_EDITOR:
			editor = 0;
			VidDrv_SetFullscreen(options[OPT_FULLSCREEN]);	// restore window size
		case ENDING_GAME:
			EndGame();
			break;
		
		case LOADING_GAME:
		case RESTARTING_LEVEL:	// don't need to do anything, it'll
			break;				// automatically just run the same level again
		
		case QUITTING_PROGRAM:
			quitapp = 1;
			break;
			
		default:
			if (result > ENTERED_LEVEL)
			{
				// entered a level from the world map
				levelcontrol.curlevel = (result - ENTERED_LEVEL);
				lprintf("entering level %d\n", levelcontrol.curlevel);
			}
			else
			{
				if (!crashflag)
					crash("gameloop() returned invalid value %d\n", result);
				else
					lprintf("gameloop() returned invalid value %d\n", result);
				quitapp = 1;
				break;
			}
	}
	
	return 0;
}




// sets up to play the demo file specified in fname
static char start_demo(int demonum)
{
FILE *fp;
int i;
int thebyte;
char filename[40];
int dmoversion;

	/* open the demo file */
	sprintf(filename, "ep%ddemo%d.dat", levelcontrol.episode, demonum);
	lprintf("- playing demo %s\n", filename);
	fp = fileopen(filename, "rb");
	if (!fp)
	{    
		return 1;
	}
	
	/* read in the header */
	if (__fgetc(fp) != 'D') goto demoHeaderCorrupt;
	if (__fgetc(fp) != 'M') goto demoHeaderCorrupt;
	if (__fgetc(fp) != 'O') goto demoHeaderCorrupt;
	if ((dmoversion = __fgetc(fp)) != DEMOVERSION)
	{
		lprintf("Wrong demo file version--expected %d, but it's %d.\n", DEMOVERSION, dmoversion);
		goto demoHeaderCorrupt;
	}
	
	if (Load_Episode(__fgetc(fp))) 
	{
		__fclose(fp);
		return 1;
	}
	levelcontrol.curlevel = __fgetc(fp);
	
	for(i=0;i<NUM_OPTIONS;i++) demo.options[i] = __fgetc(fp) ? 1:0;
	__fseek(fp, 128, SEEK_SET);
	
	if (levelcontrol.curlevel == 17)
	{
		strcpy(levelcontrol.custom_level_name, "demolvl.ck1");
		levelcontrol.play_custom_level = 1;
		demo.options[OPT_YORPFORCEFIELDS] = 0;
	}
	else
	{
		levelcontrol.play_custom_level = 0;
	}
	   
	/* load the compressed demo data into the demo_data[] array */
	for(i=0;i<DEMO_MAX_SIZE;i++)
	{
		thebyte = __fgetc(fp);
		if (thebyte < 0) break;		    // check for EOF marker
		demo.data[i] = thebyte;
		if (__feof(fp) || i>=(DEMO_MAX_SIZE-1))
		{
			crash("Error reading demo %s - corrupt or too big", filename);
			break;
		}
	}
	__fclose(fp);
	
	/* initilize some variables */
	demo.RLERunLen = 0;
	demo.data_index = 0;
	levelcontrol.gamemode = GM_DEMOPLAYBACK;
	
	initgamefirsttime();
	initgame();
   
	return 0;
	
// this label is jumped to when there's an error reading the header.
// it closes the demo file and aborts.
demoHeaderCorrupt: ;
   crash("demo %s corrupt or wrong version\n", filename);
   __fclose(fp);
   return 1;
}
