/* MISC.C
  All kinds of assorted crap :) Has most of the in-game dialog boxes
  such as the status box etc.

  Also like I said there's all kinds of assorted crap in here.
  That's why it's called "misc.c" (get it? :))
*/

#include "keen.h"
#include "misc.fdh"

#define TWIRL_SPEED        30



// start a game.
void StartGame(int episode, char *custom_path, int players)
{
int i;
	lprintf("StartGame()\n");
	levelcontrol.play_custom_level = 0;
	strcpy(levelcontrol.custom_episode_path, custom_path);
	
	if (Load_Episode(episode))
	{	// ensure requested episode is loaded
		crash("StartGame: Failed loading episode %d\n", episode);
	}
	
	numplayers = players;
	levelcontrol.curlevel = WORLD_MAP;
	levelcontrol.gameinprogress = 1;
	levelcontrol.gametoload = 0;
	Menu_ToMain();
	Menu_SetVisibility(0);
	
	memset(&overlay, 0, sizeof(overlay));
	if (levelcontrol.gamemode != GM_DEMORECORD)
	{
		levelcontrol.gamemode = GM_NORMAL;
	}
	
	// no levels are completed
	memset(levelcontrol.levels_completed, 0, sizeof(levelcontrol.levels_completed));
		
	initgamefirsttime();
	
	// set initial world-map positions
	if (!editor)
	{
		if(loadlevel(WORLD_MAP))		// to get start pos of player 0
		{
			crash("StartGame: unable to load world map.\n");
			return;
		}
		game_SetStartPositions();
		for(i=0;i<players;i++)
		{
			player[i].mapplayx = player[i].x;
			player[i].mapplayy = player[i].y;
			player[i].mapdir = DOWN;
			if (levelcontrol.episode==2) player[i].mapdir = UP;
		}
		keensleft();
	}
}


// end a game, and return to the demo/main menu screen
void EndGame(void)
{
	lprintf("EndGame()\n");
	levelcontrol.play_custom_level = 0;
	strcpy(levelcontrol.custom_episode_path, "");
	
	if (Load_Episode(1))
	{	// (for demo)
		crash("EndGame: failed loading episode files\n");
	}
	
	demo.current_demo = 1;
	memset(&overlay, 0, sizeof(overlay));
	numplayers = 1;
	newgame_number_of_players = 1;		// doesn't really affect anything but it's cleaner
	
	levelcontrol.gameinprogress = 0;
	levelcontrol.gametoload = 0;
	Menu_ToMain();
	Menu_SetVisibility(1);
	initgamefirsttime();
}


void banner(void)
{
char buf[80];

	sprintf(buf, "\n%s", REVISION);
	#ifdef __HAIKU__
		sprintf(buf, "%s  Haiku build", REVISION);
	#endif
	#ifdef WIN32
		sprintf(buf, "%s  Win32 build", REVISION);
	#endif
	#ifdef TARGET_LNX
		sprintf(buf, "%s  Linux build", REVISION);
	#endif
	lprintf("%s", buf);
	
	lprintf("\nby Caitlin Shaw, 2003-2010\n");
	lprintf("This program is released under the GNU General Public License version 3\n");
	
	lprintf("\n");
	lprintf("BY A FAN, FOR FANS. ALL \"COMMANDER KEEN\" GRAPHICS,\n");
	lprintf("SOUND, AND LEVEL FILES ARE COPYRIGHT(C) ID SOFTWARE.\n");
}


char Game_Startup(void)
{
	lprintf("> Game_Startup\n");
	Menu_Init();
	
	/* initilize/activate all drivers */
	if (Graphics_Start()) return 1;
	if (SoundDrv_Start()) return 1;
	if (KeyDrv_Start()) return 1;
	if (TimeDrv_Start()) return 1;
	return 0;
}

char episode_loaded = -1;
char episode_loaded_from_path[MAXPATHLEN] = {0};
char Load_Episode(uchar episode)
{
	if (episode_loaded != episode || \
		strcmp(episode_loaded_from_path, levelcontrol.custom_episode_path))
	{
		lprintf("Load_Episode(%d)\n", episode);
		if (loadstrings()) return 1;	// reload with ep change so custom episodes can have their own strings
		if (load_sounds(episode)) return 1;
		if (pal_load(episode)) return 1;		// load palette.ini
		if (latch_loadgraphics(episode)) return 1;
		if (LoadSpecialSprites()) return 1;
		if (loadtileattributes(episode)) return 1;
		if (initgplsprites(episode)) return 1;
		LoadFlagsIni();		// load the (custom) settings file, if present
		levelcontrol.episode = episode;
		setobjdefsprites();			// needed for loadgames
		
		episode_loaded = episode;
		strcpy(episode_loaded_from_path, levelcontrol.custom_episode_path);
	}
	return 0;
}


void Game_Shutdown(void)
{
	Graphics_Stop(); lprintf("  * Graphics driver shut down.\n");
	
	if (BitmapData)
	{
		free(BitmapData);
		lprintf("  * Bitmap buffer released to system.\n");
	}
	
	lprintf("  * Freed %d strings.\n", freestrings());
	
	if (Ini_FreeBuffer()) lprintf("  * Config file unbuffered.\n");
	
	KeyDrv_Stop(); lprintf("  * Keyboard driver shut down.\n");
	TimeDrv_Stop(); lprintf("  * Timer driver shut down.\n");
	SoundDrv_Stop(); lprintf("  * Sound driver shut down.\n");
	
	if (demo.fp)
	{
		__fclose(demo.fp);
		lprintf("  * Demo tempfile closed.\n");
	}
  
	message_dismiss();		// clean up malloc()'d storytext if necessary
	
	SDL_Quit();
	lprintf("  * SDL shut down.\n");
	lprintf("\n");
}


// return the value of a game option in the current "context"
int getoption(int optno)
{
	if (levelcontrol.gamemode != GM_DEMOPLAYBACK)
	{
		if (map.forced_options[optno] != OPTION_NOT_FORCED)
		{
			if (map.forced_options[optno]==OPTION_FORCE_ENABLE)
				return 1;
			else
				return 0;
		}
		else
		{
			return options[optno];
		}
	}
	else
	{
		return demo.options[optno];
	}
}


void sshot(char *visiblefile, char *scrollfile)
{
FILE *fp;
int x,y;

  fp = fileopen(visiblefile, "wb");
  if (!fp) return;

  for(y=0;y<WINDOW_HEIGHT;y++)
   for(x=0;x<WINDOW_WIDTH;x++)
    __fputc(getpixel(x,y), fp);

  __fclose(fp);

  fp = fileopen(scrollfile, "wb");
  if (!fp) return;

  for(y=0;y<SCROLLBUF_XSIZE;y++)
   for(x=0;x<SCROLLBUF_YSIZE;x++)
    __fputc(sb_getpixel(x,y), fp);

  __fclose(fp);
}


void AllPlayersInvisible(void)
{
int i;

  for(i=0;i<MAX_PLAYERS;i++)
  {
    if (player[i].isPlaying)
    {
      objects[player[i].useObject].onscreen = 0;
      player[i].hideplayer = 1;
    }
  }
}

// before returning to the world map after beating a level,
// this is called to determine whether or not the game has been "won"
char game_is_won(void)
{
int partcount;
int i;

       if (levelcontrol.episode==1)
       {
          /* episode 1: game is won when all parts are collected */

           // count the number of parts the players have acquired
           partcount = 0;
           for(i=0;i<MAX_PLAYERS;i++)
           {
             if (player[i].isPlaying)
             {
               if (player[i].inventory.HasJoystick) partcount++;
               if (player[i].inventory.HasBattery) partcount++;
               if (player[i].inventory.HasFuel) partcount++;
               if (player[i].inventory.HasVacuum) partcount++;
             }
           }
        
           // go to end sequence if all the parts have been got
           if (partcount >= 4)
           {
             return 1;
           }
           else return 0;
       }
       else if (levelcontrol.episode==2)
       {
         /* episode 2: game is won when all cities are saved */
         if (!levelcontrol.levels_completed[4]) return 0;
         if (!levelcontrol.levels_completed[6]) return 0;
         if (!levelcontrol.levels_completed[7]) return 0;
         if (!levelcontrol.levels_completed[13]) return 0;
         if (!levelcontrol.levels_completed[11]) return 0;
         if (!levelcontrol.levels_completed[9]) return 0;
         if (!levelcontrol.levels_completed[15]) return 0;
         if (!levelcontrol.levels_completed[16]) return 0;
         return 1;
       }
       else if (levelcontrol.episode==3)
       {
         /* episode 3: game is won when mortimer is defeated */
         if (levelcontrol.levels_completed[16])
         {
           return 1;
         }
         else
         {
           return 0;
         }
       }

return 0;
}

char game_is_over(void)
{
	if (levelcontrol.gameover) return 1;
	return 0;
}



void SetAllCanSupportPlayer(int o, int state)
{
	memset(&objects[o].cansupportplayer, state, numplayers);
}


// flag to bring up the "keens left" box
void keensleft(void)
{
  sound_play(SOUND_KEENSLEFT, PLAY_NOW);
  overlay.keenslefttime = KEENSLEFT_TIME;
}

// draw the keens left box
void drawkeensleft(void)
{
int x,y,i,p;
int boxY, boxH;
int ep3;

	// on episode 3 we have to subtract one from the map tiles
	// because the tiles start at 31, not 32 like on the other eps
	ep3 = 0;
	if (levelcontrol.episode==3) ep3 = 1;
	
	#define KEENSLEFT_X        7
	#define KEENSLEFT_Y        12
	#define KEENSLEFT_W        24
	#define KEENSLEFT_H        4
	
	boxY = KEENSLEFT_Y - (numplayers);
	boxH = KEENSLEFT_H + (numplayers * 2);
	
	dialogbox(KEENSLEFT_X<<3,boxY<<3,KEENSLEFT_W,boxH);
	font_draw(getstring("LIVES_LEFT_BACKGROUND"),(KEENSLEFT_X+1)*8,(boxY+1)*8,drawcharacter);
	font_draw(getstring("LIVES_LEFT"),((KEENSLEFT_X+7)*8)+4,(boxY+1)*8,drawcharacter);
	y = ((boxY+2)*8)+4;
	if (numplayers>1) y--;
	for(p=0;p<numplayers;p++)
	{
		x = ((KEENSLEFT_X+1)*8)+4;
		for(i=0;i<player[p].inventory.lives&&i<=10;i++)
		{
			drawsprite(x, y, PMAPDOWNFRAME + playerbaseframes[p] - ep3);
			x+=16;
		}
		y += 18;
	}

}


// a random number generator
ulong random_seed;
void srnd(ulong newseed) { random_seed = newseed; }
uint rnd(void)
{
  random_seed = random_seed * 1103515245 + 12345;
  return (uint)(random_seed / 65536) % 32768;
}


// convert raw tempfile demo.tmp to the actual RLE-compressed demo format
void save_compress_demo(void)
{
FILE *fpi, *fpo;
char l,r,j,p,f,stop;
char byt;
int i;
	lprintf("> Saving and compressing the demo...\n");
	
	// open temporary file demo.dat and the output demo file
	fpi = fileopen(DEMO_TMP, "rb");
	if (!fpi) { crash("save_compress_demo: Unable to open input file %s", DEMO_TMP); return; }
	fpo = fileopen("ep1demo1.dat", "wb");
	if (!fpo) { crash("save_compress_demo: Unable to open output file"); __fclose(fpi); return; }
	
	__fputc('D', fpo); __fputc('M', fpo); __fputc('O', fpo);
	__fputc(DEMOVERSION, fpo);
	__fputc(levelcontrol.episode, fpo);
	__fputc(levelcontrol.curlevel, fpo);
	// save the game options in effect at time of demo recording
	for(i=0;i<128;i++)
	{
		if (i < NUM_OPTIONS)
		  __fputc(options[i], fpo);
		else
		  __fputc(0, fpo);
	}
	
	// now compress and output to the real demo file
	do
	{
		l = __fgetc(fpi); r = __fgetc(fpi);
		j = __fgetc(fpi); p = __fgetc(fpi);
		f = __fgetc(fpi); stop = __fgetc(fpi);
		if (__feof(fpi) && !stop)
		{
			crash("save_compress_demo: end of file but didn't find a demo stop command\n");
			break;
		}
		
		byt = 0;
		if (l) byt |= DMO_MASK_LEFT;
		if (r) byt |= DMO_MASK_RIGHT;
		if (j) byt |= DMO_MASK_JUMP;
		if (p) byt |= DMO_MASK_POGO;
		if (f) byt |= DMO_MASK_FIRE;
		if (stop) byt |= DMO_MASK_END;
		__fputc(0, fpo);
		__fputc(0, fpo);
		__fputc(byt, fpo);
	} while(!stop);
	
	lprintf("> save_compress_demo: Resultant size %d bytes, down from %d\n", __ftell(fpo), __ftell(fpi));
	
	__fclose(fpi);
	__fclose(fpo);
	remove(DEMO_TMP);
}

// convert a two-digit hexadecimal string to an integer
uchar HexToInt(char *str)
{
	#define H2A(H) ((H>='0' && H<='9')?(H-'0'):(toupper(H)-('A'-0x0A)))
	
	return (H2A(str[0]) << 4) | H2A(str[1]);
}

// returns 1 if the program is executing on a big-endian processor
char IsBigEndian(void)
{
unsigned short int test = 0x1234;
unsigned char *ptr;

	if (sizeof(test) != 2)
	{
		crash("IsBigEndian error: 'unsigned short int' is not 16 bits.");
	}
	
	ptr = (unsigned char *)&test;
	if (ptr[0]==0x12)
	{
		return 1;
	}
	else return 0;
}

