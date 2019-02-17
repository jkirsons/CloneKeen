/* GAME.C
  Main and miscellaneous functions for in-game, contains the main
  game loop, etc.
*/

#include "keen.h"
#include "game.fdh"

//#define DEBUG

int playerbaseframes[MAX_PLAYERS] = {0,0};
uchar gl_retval;		// returned by gameloop

int max_scroll_x, max_scroll_y;

// and this is where the magic happens
uchar gameloop(void)
{
#ifdef DEBUG
ulong starttime;
ulong logictime;
int logiciter;
#endif

	gl_retval = NOREASON;		// set our default return value
	fading_to_leave_gameloop = 0;
	
	if (player[0].x==0 && player[0].y==0)
	{
		crash("No player start position--level %d ep %d\n", levelcontrol.curlevel, levelcontrol.episode);
		return NOREASON;
	}
	
	gameloop_initialize();
	
	// load game
	if (levelcontrol.gametoload)
	{
		lprintf(">gameloop: loading game %d\n", levelcontrol.gametoload);
		game_load(levelcontrol.gametoload);
		fade(FADE_IN, FADE_NORM);
		levelcontrol.gametoload = 0;
		levelcontrol.loadgame_freeze = 1;
	}
	else levelcontrol.loadgame_freeze = 0;
	
	// fire all guns immediately first time around
	gunfiretimer = (gunfirefreq+1);
	
	// if this is Mortimer's "castle", do the conversation with Mortimer.
	if (levelcontrol.episode==3 && levelcontrol.curlevel==FINAL_LEVEL && \
		levelcontrol.gamemode != GM_DEMOPLAYBACK)
	{
		seq_mortimer_showpage1();
		fade(FADE_IN, FADE_SLOW);
	}  
  
	lprintf("player start pos [%d,%d]. now entering game loop...\n", player[0].x, player[0].y);
	TimeDrv_InitTimers();
	gamedo_resettimelimit();			// for timed games
	do
	{
		if(TimeDrv_TimeToRunLogic())
		{
//			logiciter = 0;
			do
			{
				if (editor)
					editor_run();
				else
				{
					#ifdef DEBUG
						starttime = SDL_GetTicks();
					#endif
					
					gamedo_gamelogic();
					
					#ifdef DEBUG
						logictime = SDL_GetTicks() - starttime;
					#endif
				}
//				logiciter++;
			} while(TimeDrv_TimeToRunLogic());
		}
		
		// render the frame
		if (TimeDrv_TimeToRender())
		{
			#ifdef DEBUG
				starttime = SDL_GetTicks();
				gamedo_RenderScreen();
				Console_Msg("%02dblit %02dms %diter", SDL_GetTicks()-starttime, logictime, logiciter);
				lprintf("%dblit %dms %diter\n", SDL_GetTicks()-starttime, logictime, logiciter);
			#else
				gamedo_RenderScreen();
			#endif
			
			poll_events();
		}
		
		// leave loop when level is over
		if (fading_to_leave_gameloop && fadecontrol.fadeout_complete)
		{
			lprintf("Leaving game loop\n");
			break;
		}
		
	} while(!crashflag);
	
	lprintf("leaving gameloop with retval %d...\n", gl_retval);
	return gl_retval;
}


// called at the beginning of gameloop()
void gameloop_initialize(void)
{
int x,y,i;

   lprintf("gameloop_initialize()...\n");
   
	// make sure random numbers always come up the same if
	// we're running a demo (keeps monsters etc in sync)
	if (levelcontrol.gamemode != GM_NORMAL)
	{
		srnd(47);
	}
	
	if (levelcontrol.curlevel != WORLD_MAP)
	{
		overlay.keenslefttime = 0;
		sound_stop(SOUND_KEENSLEFT);
	}
	
	levelcontrol.canexit = 1;
	if (levelcontrol.episode==1)
	{	// cannot exit episode 1 levels until we collect all ship parts
		for(y=0;y<map.ysize;y++)
		for(x=0;x<map.xsize;x++)
		{
			i = map.mapdata[x][y];
			if (i==TILE_JOYSTICK || i==TILE_BATTERY || i==TILE_VACUUM || \
				i==TILE_FUEL)
			{
				levelcontrol.canexit = 0;
				break;
			}
		}
	}
	
	// lights on
	makedark(0);
	
	// initilizes Must Kill Yorps mode
	InitMKY();
	
	// if we're entering the world map, restore the position of the player
	// when he last left the world map
	if (levelcontrol.curlevel==WORLD_MAP)
	{
		for(i=0;i<numplayers;i++)
		{
			player[i].x = player[i].mapplayx;
			player[i].y = player[i].mapplayy;
		}
	}
	else
	{
		game_SetStartPositions();
	}
	
	// players start facing left if the start position
	// is on the left half of the map, else right
	// for ep1 it's always facing right (because of level08)
	for(i=0;i<MAX_PLAYERS;i++)
	{
		if (levelcontrol.curlevel==WORLD_MAP)
		{
			player[i].pdir = player[i].pshowdir = player[i].mapdir;
			gamepdo_wm_SelectFrame(i);
		}
		else
		{
			if (i==0)
			{
				if (player[i].x>>CSF>>TILE_S < (map.xsize/2) || \
					levelcontrol.episode==1 || cinematic_is_playing())
				{
					player[i].pdir = player[i].pshowdir = RIGHT;
				}
				else
				{
					player[i].pdir = player[i].pshowdir = LEFT;
				}
			}
			else player[i].pdir = player[i].pshowdir = player[0].pshowdir;
			gamepdo_SelectFrame(i);
		}
	}

  
	if (levelcontrol.bonuswarp)
		SecretLevelWarp();
	
	if (!levelcontrol.dontscroll)
	{
		scroll_to_player();
	}
	
	if (levelcontrol.gamemode==GM_DEMORECORD)
	{
		lprintf("opening demo.tmp for write...\n");
		demo.fp = fileopen(DEMO_TMP, "wb");
		lprintf("\n\n> You are recording a GAME DEMO\n");
		lprintf("> When you are ready to stop, press F1.\n");
		lprintf("> This will signal when you want the demo to fade out.\n\n");
	}
	else demo.fp = NULL;
	
	// draw map and initiate fade-in
  	if (!cinematic_is_playing())
	{
		// draw map first time
		map_redraw();
		if (!levelcontrol.bonuswarp)
			fade(FADE_IN, FADE_NORM);
		else
			fade(FADE_FLASH, FADE_NORM);
	}
}

void SecretLevelWarp(void)
{
int i,o;
	for(i=0;i<MAX_PLAYERS;i++)
	{
		player[i].mapplayx = player[i].x = BONUSLEVEL_RESPAWN_X;
		player[i].mapplayy = player[i].y = BONUSLEVEL_RESPAWN_Y;
		if (player[i].isPlaying && player[i].inventory.lives)
		{
			player[i].hideplayer = 1;
			o = spawn_object((player[i].x>>CSF>>4)<<CSF<<4,((player[i].y>>CSF>>4)+1)<<CSF<<4,OBJ_TELEPORTER);
			objects[o].ai.teleport.direction = TELEPORTING_IN;
			objects[o].ai.teleport.whichplayer = i;
			objects[o].ai.teleport.baseframe = TELEPORT_RED_BASEFRAME_EP1;
			objects[o].ai.teleport.idleframe = TELEPORT_RED_IDLEFRAME_EP1;
			sound_play(SOUND_TELEPORT, PLAY_NOW);
        }
	}
}


// gives keycard for door doortile to player p
void give_keycard(int doortile, int p, int mpx, int mpy)
{
int rb;

	sound_play(SOUND_GET_CARD, PLAY_NOW);
   
	if (doortile==DOOR_YELLOW)
	{
		player[p].inventory.HasCardYellow = 1;
		rb = PTCARDY_SPRITE;
	}
	else if (doortile==DOOR_RED)
	{
		player[p].inventory.HasCardRed = 1;
		rb = PTCARDR_SPRITE;
	}
	else if (doortile==DOOR_GREEN)
	{
		player[p].inventory.HasCardGreen = 1;
		rb = PTCARDG_SPRITE;
	}
	else if (doortile==DOOR_BLUE)
	{
		player[p].inventory.HasCardBlue = 1;
		rb = PTCARDB_SPRITE;
	}
	else
	{
		crash("give_keycard(): invalid value %d for doortile parameter.", doortile);
		rb = BLANKSPRITE;
	}
	
	if (mpx || mpy)
	{
		risebonus(rb, (mpx<<4<<CSF), (mpy<<4<<CSF)-(4<<CSF));
	}
}

// take away the specified keycard from player p
void take_keycard(int doortile, int p)
{
	if (doortile==DOOR_YELLOW) player[p].inventory.HasCardYellow = 0;
	else if (doortile==DOOR_RED) player[p].inventory.HasCardRed = 0;
	else if (doortile==DOOR_GREEN) player[p].inventory.HasCardGreen = 0;
	else if (doortile==DOOR_BLUE) player[p].inventory.HasCardBlue = 0;
	else
	{
		crash("take_keycard(): invalid value %d for doortile parameter.", doortile);
	}   
}

// unregisters all animated tiles with baseframe tile
void unregister_animtiles(int tile)
{
int i;
     for(i=0;i<MAX_ANIMTILES-1;i++)
     {
        if (animtiles[i].baseframe == tile)
        {
           animtiles[i].slotinuse = 0;
        }
     }
}

void open_door(int doortile, int doorsprite, int mpx, int mpy, int cp)
{
int o,chgtotile;

   sound_play(SOUND_DOOR_OPEN, PLAY_NOW);
   take_keycard(doortile, cp);

   /* erase door from map */
   if (levelcontrol.episode==3)
   {
     chgtotile = map.mapdata[mpx-1][mpy];
   }
   else
   {
	 chgtotile = tiles[map.mapdata[mpx][mpy]].chgtile;
   }
   map_chgtile(mpx, mpy, chgtotile);
   map_chgtile(mpx, mpy+1, chgtotile);
   // erase any ray-blockers so the "door" doesn't block shots anymore
   map.objectlayer[mpx][mpy] = 0;
   map.objectlayer[mpx][mpy+1] = 0;

   // replace the door tiles with a door object, which will do the animation
   o = spawn_object(mpx<<TILE_S<<CSF, mpy<<TILE_S<<CSF, OBJ_DOOR);
   objects[o].sprite = doorsprite;      
}

void take_all_keycards(void)
{
int i;
	for(i=0;i<MAX_PLAYERS;i++)
	{
		if (player[i].isPlaying)
		{
			take_keycard(DOOR_YELLOW, i);
			take_keycard(DOOR_RED, i);
			take_keycard(DOOR_GREEN, i);
			take_keycard(DOOR_BLUE, i);
		}
	}
}

char can_exit(int cp)
{
	if (!levelcontrol.canexit) return 0;
	if (player[cp].ppogostick) return 0;
	if (getoption(OPT_MUSTKILLYORPS) && levelcontrol.numyorps > 0)
	{
		return 0;
	}
	if (levelcontrol.sparks_left > 0)
	{
		Console_Msg("You must destroy the Tantalus Ray!");
		Console_SetTimeout(10);
		return 0;
	}
	return 1;
}

void initplayers(void)
{
int i;
	for(i=numplayers;i<MAX_PLAYERS;i++) player[i].isPlaying = 0;
	for(i=0;i<numplayers;i++)
	{
		// reset walk-frame widths, which the exit anim screws up
		sprites[playerbaseframes[i]+0].xsize = PLAYERSPRITE_WIDTH;
		sprites[playerbaseframes[i]+1].xsize = PLAYERSPRITE_WIDTH;
		sprites[playerbaseframes[i]+2].xsize = PLAYERSPRITE_WIDTH;
		sprites[playerbaseframes[i]+3].xsize = PLAYERSPRITE_WIDTH;
		player[i].isPlaying = 1;
		player[i].useObject = i;
		player[i].dpadcount = player[i].dpadlastcount = 0;
		player[i].hideplayer = 0;
		player[i].mounted = 0;
		player[i].ppogostick = 0;
		player[i].pjumping = 0;
		player[i].pfalling = 0;
		player[i].pwalking = player[i].playspeed = 0;
		player[i].pinertia_x = player[i].pinertia_y = 0;
		player[i].playpushed_x = 0;
		player[i].pfiring = 0;
		player[i].psliding = player[i].psemisliding = 0;
		player[i].pdie = 0;
		player[i].pfrozentime = 0;
		player[i].dpadcount = player[i].dpadlastcount = 0;
		player[i].ankhtime = 0;
		player[i].keyprocstate = 0;         // KPROC_IDLE
		player[i].pjustjumped = player[i].pjustfell = 0;
	}
}

void initgame(void)
{
int x, y, i;

	lprintf("initgame()...\n");
	
	animtiletimer = curanimtileframe = 0;
	
	levelcontrol.PlatExtending = 0;
	levelcontrol.vibratetime = 0;
	message_SetDismissalCallback(NULL);
	message_dismiss();
	
	// set gun/ice cannon fire freq
	if (levelcontrol.episode==1)
	{
		gunfirefreq = ICECANNON_FIRE_FREQ;
	}
	else
	{
		gunfirefreq = GUN_FIRE_FREQ;
	}
	
	// all objects -> not exist
	for(i=0;i<MAX_OBJECTS;i++) objects[i].exists = 0;
	
	// clear out AnimTileInUse array
	for(y=0;y<ATILEINUSE_SIZEX-1;y++)
	{
		for(x=0;x<ATILEINUSE_SIZEY-1;x++)
		{
			AnimTileInUse[x][y] = 0;
		}
	}
	// set all animated tile slots to "not in use"
	map_unregister_all_animtiles();
	
	initplayers();
	// each player is tied to a "puppet" object.
	// initilize objects used by players.
	for(i=0;i<numplayers;i++)
	{
		if (player[i].isPlaying)
		{
			objects[player[i].useObject].exists = 1;
			objects[player[i].useObject].onscreen = 1;
			objects[player[i].useObject].type = OBJ_PLAYER;
			objects[player[i].useObject].sprite = 0;
			objects[player[i].useObject].onscreen = 1;
			objects[player[i].useObject].AssociatedWithPlayer = i;
			objects[player[i].useObject].honorPriority = 1;
			objects[player[i].useObject].canbezapped = 0;
			highest_objslot = player[i].useObject + 1;
		}
	}
	
	// initilize some game variables
	memset(last_keytable, 1, KEYTABLE_SIZE);
	
	levelcontrol.level_done = LEVEL_NOT_DONE;
	levelcontrol.gameover = 0;
	
	animtiletimer = curanimtileframe = 0;
	
	map_resetscroll();
}

void initgamefirsttime(void)
{
int i;

	lprintf("initgamefirsttime()...\n");
	
	setobjdefsprites();
	sound_resume();				// just in case
	
	memset(player, 0, sizeof(player));
	levelcontrol.bonuswarp = 0;
	
	cinematics_stop();
	message_dismiss();

   for(i=0;i<MAX_PLAYERS;i++)
   {
       player[i].inventory.extralifeat = 20000;
       player[i].inventory.lives = PLAYER_STARTLIVES;
	   if (getoption(OPT_HARD)) player[i].inventory.lives--;
       player[i].godmode = 0;

       if (levelcontrol.episode==1)
       {
         player[i].inventory.charges = 0;
       }
       else if (levelcontrol.episode==2)
       {
         player[i].inventory.charges = 3;
       }
       else
       {
         player[i].inventory.charges = 5;
       }
	   // give a shitload of ammo when playing or recording a demo
       if (levelcontrol.gamemode != GM_NORMAL)
		player[i].inventory.charges = 100;
		
       // start with pogo stick in all episodes but 1
       if (levelcontrol.episode!=1 || levelcontrol.gamemode != GM_NORMAL)
         { player[i].inventory.HasPogo = 1; }
       else
         { player[i].inventory.HasPogo = 0; }
   }

	levelcontrol.dontscroll = 0;
	primaryplayer = 0;	
}


// initilize default sprites for objects
void setobjdefsprites(void)
{
	lprintf("setobjdefsprites...\n");
	memset(objdefsprites, 0, sizeof(objdefsprites));
	
	objdefsprites[OBJ_AUTORAY] = ENEMYRAYEP3;
	objdefsprites[OBJ_AUTORAY_V] = RAY_VERT_EP3;
		
	objdefsprites[OBJ_DOOR] = DOOR_YELLOW_SPRITE;
	objdefsprites[OBJ_TELEPORTER] = OBJ_TELEPORTER_DEFSPRITE;
	
	objdefsprites[OBJ_SECTOREFFECTOR] = BLANKSPRITE;
	objdefsprites[OBJ_GOTPOINTS] = PT500_SPRITE;
	
	if (levelcontrol.episode==1)
	{
		objdefsprites[OBJ_YORP] = OBJ_YORP_DEFSPRITE;
		objdefsprites[OBJ_GARG] = OBJ_GARG_DEFSPRITE;
		objdefsprites[OBJ_BUTLER] = OBJ_BUTLER_DEFSPRITE;
		objdefsprites[OBJ_TANK] = OBJ_TANK_DEFSPRITE;
		objdefsprites[OBJ_ICECHUNK] = OBJ_ICECHUNK_DEFSPRITE;
		objdefsprites[OBJ_ICEBIT] = OBJ_ICEBIT_DEFSPRITE;
		objdefsprites[OBJ_ROPE] = OBJ_ROPE_DEFSPRITE;
		
		objdefsprites[OBJ_RAY] = OBJ_RAY_DEFSPRITE_EP1;
		objdefsprites[OBJ_VORT] = OBJ_VORT_DEFSPRITE_EP1;
		// make ice cannon markers show up in level editor
		objdefsprites[OBJ_ICECANNON] = OBJ_ICECHUNK_DEFSPRITE;
	}
	else if (levelcontrol.episode==2)
	{
		objdefsprites[OBJ_WALKER] = OBJ_WALKER_DEFSPRITE;
		objdefsprites[OBJ_TANKEP2] = OBJ_TANKEP2_DEFSPRITE;
		objdefsprites[OBJ_VORTELITE] = OBJ_VORTELITE_DEFSPRITE;
		
		objdefsprites[OBJ_RAY] = OBJ_RAY_DEFSPRITE_EP2;
		objdefsprites[OBJ_VORT] = OBJ_VORT_DEFSPRITE_EP2;
		objdefsprites[OBJ_PLATFORM] = OBJ_PLATFORM_DEFSPRITE_EP2;
		objdefsprites[OBJ_BABY] = OBJ_BABY_DEFSPRITE_EP2;
		objdefsprites[OBJ_SPARK] = OBJ_SPARK_DEFSPRITE_EP2;
	}
	else if (levelcontrol.episode==3)
	{
		objdefsprites[OBJ_FOOB] = OBJ_FOOB_DEFSPRITE;
		objdefsprites[OBJ_NINJA] = OBJ_NINJA_DEFSPRITE;
		objdefsprites[OBJ_MOTHER] = OBJ_MOTHER_DEFSPRITE;
		objdefsprites[OBJ_MEEP] = OBJ_MEEP_DEFSPRITE;
		objdefsprites[OBJ_BALL] = OBJ_BALL_DEFSPRITE;
		objdefsprites[OBJ_JACK] = OBJ_JACK_DEFSPRITE;
		objdefsprites[OBJ_NESSIE] = OBJ_NESSIE_DEFSPRITE;
		
		objdefsprites[OBJ_RAY] = OBJ_RAY_DEFSPRITE_EP3;
		objdefsprites[OBJ_VORT] = OBJ_VORT_DEFSPRITE_EP3;
		objdefsprites[OBJ_PLATFORM] = OBJ_PLATFORM_DEFSPRITE_EP3;
		objdefsprites[OBJ_PLATVERT] = OBJ_PLATFORM_DEFSPRITE_EP3;
		objdefsprites[OBJ_BABY] = OBJ_BABY_DEFSPRITE_EP3;
	}
}


uchar spawn_object(int x, int y, int otype)
{
int i;

	// find an unused object slot
	for(i=1;i<MAX_OBJECTS;i++)
	{
		if (!objects[i].exists && objects[i].type != OBJ_PLAYER)
		{
			objects[i].x = x;
			objects[i].y = y;
			objects[i].type = otype;
			objects[i].sprite = objdefsprites[otype];
			objects[i].exists = 1;
			objects[i].needinit = 1;
			objects[i].dead = 0;
			objects[i].onscreen = 0;
			objects[i].hasbeenonscreen = 0;
			objects[i].zapped = 0;
			objects[i].canbezapped = 0;
			objects[i].inhibitfall = 0;
			objects[i].honorPriority = 1;
			
			SetAllCanSupportPlayer(i, 0);
			
			if (i >= highest_objslot)
			{
				highest_objslot = i+1;
			}
			return i;
		}
	}
	
	// object could not be created
	crash("Object of type %d could not be created at %d,%d (out of object slots)",otype,x,y);
	return 0;
}

void delete_object(int o)
{
	if (objects[o].exists)
	{
		objects[o].exists = 0;
		if (o+1==highest_objslot) highest_objslot--;
	}
}

void delete_all_objects(void)
{
int i;
	for(i=0;i<MAX_OBJECTS;i++)
	{
		if (objects[i].exists && objects[i].type != OBJ_PLAYER)
			delete_object(i);
	}
	recalc_highest_objslot();
}
void recalc_highest_objslot(void)
{
int i;
	highest_objslot = 0;
	for(i=MAX_OBJECTS-1;i>=0;i--)
	{
		if (objects[i].exists)
		{
			highest_objslot = i+1;
			break;
		}
	}
}

void killobject(int o)
{
	if (objects[o].exists)
	{
		if (objects[o].type==OBJ_PLAYER)
		{
			killplayer(o);
		}
		else
		{
			if (objects[o].zapped < 500 && objects[o].canbezapped)
				objects[o].zapped += 500;
		}
	}
}

// anything (players/enemies) occupying the map tile at [mpx,mpy] is killed
void kill_all_intersecting_tile(int mpx, int mpy)
{
int xpix,ypix;
int i;
	xpix = mpx<<TILE_S<<CSF;
	ypix = mpy<<TILE_S<<CSF;
	for(i=0;i<highest_objslot;i++)
	{
		if (objects[i].exists)
		{
			if (xpix <= objects[i].x && xpix+(16<<CSF) >= objects[i].x)
			{
				if (ypix <= objects[i].y && ypix+(16<<CSF) >= objects[i].y)
				{
					killobject(i);
				}
			}
		}
	}
}

// returns whether pix position x,y is a stop point for object o.
// stop points are invisible blockers that act solid only to certain
// kinds of enemies. They're used to help make the enemies seem smarter
// and keep them from falling off certain platforms. Stoppoints are manually
// placed from fileio.c.
char IsStopPoint(int x, int y, int o)
{
	switch(objects[o].type)
	{
		case OBJ_YORP:
		case OBJ_GARG:
		case OBJ_MOTHER:
		case OBJ_VORT:
		case OBJ_VORTELITE:
		case OBJ_TANK:
		case OBJ_TANKEP2:
			if (getlevelat(x,y)==ENEMY_STOPPOINT) return 1;
		case OBJ_WALKER:
		case OBJ_PLATFORM:
		case OBJ_PLATVERT:
		case OBJ_BABY:
			if (IsDoor(getmaptileat(x,y))) return 1;
		break;
		
		case OBJ_RAY:
			if (getoption(OPT_DOORSBLOCKRAY))
			{
				if (IsDoor(getmaptileat(x,y))) return 1;
			}
		break;
		
		case OBJ_BALL:
			if (getlevelat(x,y)==BALL_NOPASSPOINT) return 1;
			if (IsDoor(getmaptileat(x,y))) return 1;
		break;
	}
	
	return 0;
}


// returns nonzero if object1 overlaps object2
char hitdetect(int object1, int object2)
{
int s1, s2;
unsigned int rect1x1, rect1y1, rect1x2, rect1y2;
unsigned int rect2x1, rect2y1, rect2x2, rect2y2;

  // get the sprites used by the two objects
  s1 = objects[object1].sprite;
  s2 = objects[object2].sprite;

  // get the bounding rectangle of the first object
  rect1x1 = objects[object1].x + sprites[s1].bboxX1;
  rect1y1 = objects[object1].y + sprites[s1].bboxY1;
  rect1x2 = objects[object1].x + sprites[s1].bboxX2;
  rect1y2 = objects[object1].y + sprites[s1].bboxY2;

  // get the bounding rectangle of the second object
  rect2x1 = objects[object2].x + sprites[s2].bboxX1;
  rect2y1 = objects[object2].y + sprites[s2].bboxY1;
  rect2x2 = objects[object2].x + sprites[s2].bboxX2;
  rect2y2 = objects[object2].y + sprites[s2].bboxY2;

  // find out if the rectangles overlap
  if ((rect1x1 < rect2x1) && (rect1x2 < rect2x1)) return 0;
  if ((rect1x1 > rect2x2) && (rect1x2 > rect2x2)) return 0;
  if ((rect1y1 < rect2y1) && (rect1y2 < rect2y1)) return 0;
  if ((rect1y1 > rect2y2) && (rect1y2 > rect2y2)) return 0;

  return 1;
}

void killplayer(int theplayer)
{
	if (player[theplayer].godmode || keytable[KTAB]) return;
	if (player[theplayer].ankhtime) return;
	if (levelcontrol.level_done) return;
	
	if (!player[theplayer].pdie)
	{
		player[theplayer].pdie = PDIE_DYING;
		player[theplayer].pdieframe = 0;
		player[theplayer].pdietimer = 0;
		player[theplayer].pdietillfly = DIE_TILL_FLY_TIME;
		player[theplayer].pdie_xvect = rnd()%(DIE_MAX_XVECT*2);
		player[theplayer].pdie_xvect -= DIE_MAX_XVECT;
		levelcontrol.died_cause_time_over = 0;
		gamepdo_SelectFrame(theplayer);
		sound_play(SOUND_KEEN_DIE, PLAY_NOW);
	}
}

// undo a killplayer()
void reviveplayer(int theplayer)
{
	if (player[theplayer].pdie)
	{
		player[theplayer].pdie = 0;
		sound_stop(SOUND_KEEN_DIE);
	}
}

// freeze (or stun) the player
void freezeplayer(int theplayer)
{
	if (player[theplayer].godmode || keytable[KTAB]) return;
	if (player[theplayer].ankhtime) return;
	// give the player a little "kick"
	player[theplayer].pjumptime = PJUMP_NORMALTIME_1;
	player[theplayer].pjumpupdecreaserate = PJUMP_UPDECREASERATE_1;
	player[theplayer].pjumpupspeed = 15;
	player[theplayer].pjumping = PJUMPUP;
	player[theplayer].pjumpupspeed_decreasetimer = 0;
	player[theplayer].pjustjumped = 1;
	
	// and freeze him (stun him on ep2/3)
	player[theplayer].pfrozentime = PFROZEN_TIME;
	player[theplayer].pfrozenframe = 0;
	player[theplayer].pfrozenanimtimer = 0;
	player[theplayer].ppogostick = 0;
}

// called if the player touches the exit door.
// door_left_side_x is the X coordinate, in tiles, of
// the tile making up the left-hand side of the door.
void PlayerTouchedExit(int theplayer, int door_left_side_x)
{
	levelcontrol.exitXpos = (door_left_side_x+2)<<TILE_S;
	
	if (!player[theplayer].pjumping && !player[theplayer].pfalling\
		&& !player[theplayer].ppogostick && \
		levelcontrol.level_done==LEVEL_NOT_DONE)
	{
		// don't allow player to walk through the exit if he's standing
		// on an object such as a platform or an enemy
		if (player[theplayer].psupportingobject)
		{
			return;
		}
			
		// if player has ankh shut it off
		if (player[theplayer].ankhtime)
		{
			player[theplayer].ankhtime = 0;
			objects[player[theplayer].ankhshieldobject].exists = 0;
		}
		
		player[theplayer].ppogostick = 0;
		
		sound_play(SOUND_LEVEL_DONE, PLAY_NOW);
		levelcontrol.level_done = LEVEL_DONE_WALK;
		levelcontrol.level_done_timer = 0;
		levelcontrol.level_finished_by = theplayer;
	}
}

void endlevel(int reason_for_leaving)
{
	if (gl_retval==NOREASON || \
		reason_for_leaving==QUITTING_PROGRAM || reason_for_leaving==ENDING_GAME)
	{
		fading_to_leave_gameloop = 1;
		fade(FADE_OUT, FADE_NORM);
		
		gl_retval = reason_for_leaving;
		
		lprintf("endlevel(%d)\n", reason_for_leaving);
		levelcontrol.bonuswarp = 0;
	}
	else
	{
		//lprintf("> [ignoring endlevel(%d) because gl_retval=%d]\n", reason_for_leaving, gl_retval);
	}
}

void SetGameOver(void)
{
	if (!levelcontrol.gameover)
	{
		levelcontrol.gameover = 1;
		sound_play(SOUND_GAME_OVER, PLAY_NOW);
	}
}


// this is so objects can block the player,
// player can stand on them, etc.
// x and y are the CSFed coordinates to check (e.g. playx and playy)
// returns nonzero if there is a solid object
// at that point
char checkobjsolid(int x, int y, int cp)
{
  int o;

   for(o=1;o<highest_objslot;o++)
   {
      if (objects[o].exists && objects[o].cansupportplayer[cp])
      {
        if (x >= objects[o].x+sprites[objects[o].sprite].bboxX1)
          if (x <= objects[o].x+sprites[objects[o].sprite].bboxX2)
            if (y >= objects[o].y+sprites[objects[o].sprite].bboxY1)
              if (y <= objects[o].y+sprites[objects[o].sprite].bboxY2)
                return o;
      }
   }
 return 0;
}

// returns nonzero if tile T is a door
char IsDoor(int t)
{
   if (levelcontrol.episode!=3)
   {  // episode 1-2
        if (t==DOOR_YELLOW || t==DOOR_YELLOW+1 || t==DOOR_RED || t==DOOR_RED+1)
			return 1;
        if (t==DOOR_GREEN || t==DOOR_GREEN+1 || t==DOOR_BLUE || t==DOOR_BLUE+1)
			return 1;
   }
   else
   {  // episode 3
        if (t==DOOR_YELLOW_EP3 || t==DOOR_YELLOW_EP3+1) return 1;
		if (t==DOOR_RED_EP3 || t==DOOR_RED_EP3+1) return 1;
        if (t==DOOR_GREEN_EP3 || t==DOOR_GREEN_EP3+1) return 1;
        if (t==DOOR_BLUE_EP3 || t==DOOR_BLUE_EP3+1) return 1;
   }
   return 0;
}

// returns 1 if player cp has the card to door t
char CheckDoorBlock(int t, int cp)
{
   if (levelcontrol.episode!=3)
   {  // episode 1-2
        if (t==DOOR_YELLOW || t==DOOR_YELLOW+1)
        {
          if (!player[cp].inventory.HasCardYellow)
          {
            player[cp].blockedby = DOOR_YELLOW;
            return 1;
          }
        }
        else if (t==DOOR_RED || t==DOOR_RED+1)
        {
          if (!player[cp].inventory.HasCardRed)
          {
            player[cp].blockedby = DOOR_RED;
            return 1;
          }
        }
        else if (t==DOOR_GREEN || t==DOOR_GREEN+1)
        {
          if (!player[cp].inventory.HasCardGreen)
          {
            player[cp].blockedby = DOOR_GREEN;
            return 1;
          }
        }
        else if (t==DOOR_BLUE || t==DOOR_BLUE+1)
        {
          if (!player[cp].inventory.HasCardBlue)
          {
            player[cp].blockedby = DOOR_BLUE;
            return 1;
          }
        }
   }
   else
   {  // episode 3
        if (t==DOOR_YELLOW_EP3 || t==DOOR_YELLOW_EP3+1)
        {
          if (!player[cp].inventory.HasCardYellow)
          {
            player[cp].blockedby = DOOR_YELLOW_EP3;
            return 1;
          }
        }
        else if (t==DOOR_RED_EP3 || t==DOOR_RED_EP3+1)
        {
          if (!player[cp].inventory.HasCardRed)
          {
            player[cp].blockedby = DOOR_RED_EP3;
            return 1;
          }
        }
        else if (t==DOOR_GREEN_EP3 || t==DOOR_GREEN_EP3+1)
        {
          if (!player[cp].inventory.HasCardGreen)
          {
            player[cp].blockedby = DOOR_GREEN_EP3;
            return 1;
          }
        }
        else if (t==DOOR_BLUE_EP3 || t==DOOR_BLUE_EP3+1)
        {
          if (!player[cp].inventory.HasCardBlue)
          {
            player[cp].blockedby = DOOR_BLUE_EP3;
            return 1;
          }
        }
   }

   return 0;
}

// checks if tile at (x,y) is solid to the player walking left into it.
// returns 1 and sets blockedby if so.
char checkissolidl(int x, int y, int cp)
{
int t;
  t = getmaptileat(x, y);
  if (tiles[t].solidl)
  {
    player[cp].blockedby = t;
    return 1;
  }
  if (checkobjsolid(x<<CSF,y<<CSF,cp))
  {
    player[cp].blockedby = 0;
    return 1;
  }
  else
  {
    // don't let player walk through doors he doesn't have the key to
    if (CheckDoorBlock(t, cp))
    {
      return 1;
    }
  }
  return 0;
}

// checks if tile is a "floor" (solidfall).
char checkissolidd(int t, int cp)
{
	if (tiles[t].solidfall) return 1;
	return CheckDoorBlock(t, cp);	// let player stand on top of doors
}

// checks if tile at (x,y) is solid to the player walking right into it.
// returns 1 and sets blockedby if so.
char checkissolidr(int x, int y, int cp)
{
int t;
  t = getmaptileat(x, y);
  if (tiles[t].solidr)
  {
    player[cp].blockedby = t;
    return 1;
  }
  else if (checkobjsolid(x<<CSF,y<<CSF,cp))
  {
    player[cp].blockedby = 0;
    return 1;
  }
  else
  {
    // don't let player walk through doors he doesn't have the key to
    if (CheckDoorBlock(t, cp))
    {
      return 1;
    }
  }
  return 0;
}


void risebonus(int spr, int x, int y)
{
int o;
	if (getoption(OPT_RISEBONUS))
	{
		o = spawn_object(x, y, OBJ_GOTPOINTS);
		objects[o].sprite = spr;
	}
}

void incscore(int cp, int numpts)
{
	player[cp].inventory.score += numpts;
	
	// check if score is > than "extra life at"
	if (player[cp].inventory.score >= player[cp].inventory.extralifeat)
	{
		sound_stop(SOUND_GET_BONUS);
		sound_play(SOUND_EXTRA_LIFE, PLAY_NOW);
		player[cp].inventory.lives++;
		player[cp].inventory.extralifeat += 20000;
	}
}

void getbonuspoints(int cp, int numpts, int mpx, int mpy)
{
int spr;
int x,y;

	sound_play(SOUND_GET_BONUS, PLAY_NOW);
	incscore(0, numpts);
	
	switch(numpts)
	{
		case 100: spr = PT100_SPRITE; break;
		case 200: spr = PT200_SPRITE; break;
		case 500: spr = PT500_SPRITE; break;
		case 1000: spr = PT1000_SPRITE; break;
		case 5000: spr = PT5000_SPRITE; break;
		default: spr = 0;
	}
	if (spr)
	{
		x = mpx<<4<<CSF; y = mpy<<4<<CSF;
		risebonus(spr, x-(2<<CSF), y-(2<<CSF));
	}
}


// have keen pick up the goodie at pixel position (px, py)
// (relative to top of map)
void keen_get_goodie(int px, int py, int theplayer)
{
int mpx,mpy,t;
char isUnknown;

	mpx = (px>>TILE_S); mpy = (py>>TILE_S);
	t = map.mapdata[mpx][mpy];
	
	// when handling animated tiles, always pretend it's the first frame
	if (tiles[t].isAnimated) t -= tiles[t].animOffset;
	
	if (tiles[t].pickupable)
	{  // pick up the goodie, i.e. erase it from the map
		map_chgtile(mpx, mpy, tiles[t].chgtile);
	}
	
	// give points if goodie is worth any
	if (tiles[t].points)
		getbonuspoints(theplayer, tiles[t].points, mpx, mpy);
	
	if (tiles[t].lethal)
	{  // whoah, this "goodie" isn't so good...
		killplayer(theplayer);
		return;
	}
	
	// do whatever the goodie is supposed to do...
	if (levelcontrol.episode==1)
	{
		isUnknown = procgoodie_ep1(t, mpx, mpy, theplayer);
	}
	else if (levelcontrol.episode==2)
	{
		isUnknown = procgoodie_ep2(t, mpx, mpy, theplayer);
	}
	else if (levelcontrol.episode==3)
	{
		isUnknown = procgoodie_ep3(t, mpx, mpy, theplayer);
	}
	
	if (isUnknown && !tiles[t].pickupable)
	{
		crash("keen_get_goodie: Unknown goodie tile %d", t);
	}
}


char procgoodie_ep1(int t, int mpx, int mpy, int theplayer)
{
   switch(t)
   {
    // keycards
    case TILE_KEYYELLOW: case TILE_KEYYELLOW_DARK:
		give_keycard(DOOR_YELLOW, theplayer, mpx, mpy); break;
    case TILE_KEYRED: case TILE_KEYRED_DARK:
		give_keycard(DOOR_RED, theplayer, mpx, mpy); break;
    case TILE_KEYGREEN: case TILE_KEYGREEN_DARK:
		give_keycard(DOOR_GREEN, theplayer, mpx, mpy); break;
    case TILE_KEYBLUE: case TILE_KEYBLUE_DARK:
		give_keycard(DOOR_BLUE, theplayer, mpx, mpy); break;
	
    case DOOR_YELLOW:
    case DOOR_YELLOW+1:
		if (player[theplayer].inventory.HasCardYellow)
			open_door(DOOR_YELLOW, DOOR_YELLOW_SPRITE, mpx, mpy-(t-DOOR_YELLOW), theplayer);
		break;
    case DOOR_RED:
    case DOOR_RED+1:
		if (player[theplayer].inventory.HasCardRed)
			open_door(DOOR_RED, DOOR_RED_SPRITE, mpx, mpy-(t-DOOR_RED), theplayer);
		break;
    case DOOR_GREEN:
    case DOOR_GREEN+1:
		if (player[theplayer].inventory.HasCardGreen)
			open_door(DOOR_GREEN, DOOR_GREEN_SPRITE, mpx, mpy-(t-DOOR_GREEN), theplayer);
		break;
    case DOOR_BLUE:
    case DOOR_BLUE+1:
		if (player[theplayer].inventory.HasCardBlue)
			open_door(DOOR_BLUE, DOOR_BLUE_SPRITE, mpx, mpy-(t-DOOR_BLUE), theplayer);
		break;
    
    case TILE_RAYGUN:           // raygun
	case TILE_RAYGUN_DARK:
	     risebonus(GUNUP_SPRITE, (mpx<<4<<CSF), (mpy<<4<<CSF)-(2<<CSF));
         player[theplayer].inventory.charges += 5;
		 sound_play(SOUND_GET_ITEM, PLAY_NOW);
    break;
    case TILE_POGO:           // the Holy Pogo Stick
         player[theplayer].inventory.HasPogo = 1;
		 sound_play(SOUND_GET_ITEM, PLAY_NOW);
    break;
    
    case TILE_JOYSTICK:
     player[theplayer].inventory.HasJoystick = 1;
	 levelcontrol.canexit = 1;
     sound_play(SOUND_GET_PART, PLAY_NOW);
    break;
    case TILE_BATTERY:
     player[theplayer].inventory.HasBattery = 1;
	 levelcontrol.canexit = 1;
     sound_play(SOUND_GET_PART, PLAY_NOW);
    break;
    case TILE_VACUUM:
     player[theplayer].inventory.HasVacuum = 1;
	 levelcontrol.canexit = 1;
     sound_play(SOUND_GET_PART, PLAY_NOW);
    break;
    case TILE_FUEL:
     player[theplayer].inventory.HasFuel = 1;
	 levelcontrol.canexit = 1;
     sound_play(SOUND_GET_PART, PLAY_NOW);
    break;

    // in-level teleporter
    // (in level13.ck1 that takes you to the bonus level)
    case 481:
    case 494:
        endlevel(FOUND_SECRET_LEVEL);
    break;

    // yorp/garg statue
    case 486: case 435: case 436:
      youseeinyourmind(mpx, mpy);
      break;
	  
    // exit door
    case 159:
      if (can_exit(theplayer))
      {
		PlayerTouchedExit(theplayer, mpx);
      }
	  player[theplayer].standingonexit = 1;
	  break;
	
	case 161: player[theplayer].standingonexit = 1; break; //right side of door
	

    // we fell off the bottom of the map
    case TILE_FELLOFFMAP: felloffmap(theplayer); break;

    default: return 1;
   }

return 0;
}

char procgoodie_ep2(int t, int mpx, int mpy, int theplayer)
{
   switch(t)
   {
    // keycards
    case TILE_KEYYELLOW: case TILE_KEYYELLOW_DARK:
		give_keycard(DOOR_YELLOW, theplayer, mpx, mpy); break;
    case TILE_KEYRED: case TILE_KEYRED_DARK:
		give_keycard(DOOR_RED, theplayer, mpx, mpy); break;
    case TILE_KEYGREEN: case TILE_KEYGREEN_DARK:
		give_keycard(DOOR_GREEN, theplayer, mpx, mpy); break;
    case TILE_KEYBLUE: case TILE_KEYBLUE_DARK:
		give_keycard(DOOR_BLUE, theplayer, mpx, mpy); break;
		
    case DOOR_YELLOW:
    case DOOR_YELLOW+1:
		if (player[theplayer].inventory.HasCardYellow)
			open_door(DOOR_YELLOW, DOOR_YELLOW_SPRITE, mpx, mpy-(t-DOOR_YELLOW), theplayer);
		break;
    case DOOR_RED:
    case DOOR_RED+1:
		if (player[theplayer].inventory.HasCardRed)
			open_door(DOOR_RED, DOOR_RED_SPRITE, mpx, mpy-(t-DOOR_RED), theplayer);
		break;
    case DOOR_GREEN:
    case DOOR_GREEN+1:
		if (player[theplayer].inventory.HasCardGreen)
			open_door(DOOR_GREEN, DOOR_GREEN_SPRITE, mpx, mpy-(t-DOOR_GREEN), theplayer);
		break;
    case DOOR_BLUE:
    case DOOR_BLUE+1:
		if (player[theplayer].inventory.HasCardBlue)
			open_door(DOOR_BLUE, DOOR_BLUE_SPRITE, mpx, mpy-(t-DOOR_BLUE), theplayer);
		break;
    
    case TILE_RAYGUN:
    case TILE_RAYGUN_GREENBG:
	case TILE_RAYGUN_DARK:
		player[theplayer].inventory.charges += 5;
		risebonus(GUNUP_SPRITE, (mpx<<4<<CSF), (mpy<<4<<CSF)-(2<<CSF));
		sound_play(SOUND_GET_ITEM, PLAY_NOW);
    break;

    case TILE_POGO:   	   // pogo (not used in ep2 but what the hell)
		player[theplayer].inventory.HasPogo = 1;
		sound_play(SOUND_GET_ITEM, PLAY_NOW);
	break;

    case TILE_ELDERSWITCH:     // switch for vorticon elder
      VorticonElder(mpx, mpy);
    break;
    
    // exit door
    case 159:
      player[theplayer].standingonexit = 1;
      if (can_exit(theplayer))
      {
        PlayerTouchedExit(theplayer, mpx);
      }
    break;

    // we fell off the bottom of the map
    case TILE_FELLOFFMAP: felloffmap(theplayer); break;

    default: return 1;
   }
   
return 0;
}

char procgoodie_ep3(int t, int mpx, int mpy, int theplayer)
{
   switch(t)
   {		 
    case 149:    // ankh
    case 149+13:
    case 149+13+13:
    case 149+13+13+13:
    case 149+13+13+13+13:
    case 149+13+13+13+13+13:
         GiveAnkh(theplayer);
         break;
    case 150:   // raygun charge
    case 150+13:
    case 150+13+13:
    case 150+13+13+13:
    case 150+13+13+13+13:
    case 150+13+13+13+13+13:
      player[theplayer].inventory.charges++;
      sound_play(SOUND_GET_ITEM, PLAY_NOW);
	  risebonus(SHOTUP_SPRITE, (mpx<<4<<CSF), (mpy<<4<<CSF)-(2<<CSF));
      break;
    case 151:   // raygun
    case 151+13:
    case 151+13+13:
    case 151+13+13+13:
    case 151+13+13+13+13:
    case 151+13+13+13+13+13:
      player[theplayer].inventory.charges += 5;
      sound_play(SOUND_GET_ITEM, PLAY_NOW);
	  risebonus(GUNUP_SPRITE, (mpx<<4<<CSF), (mpy<<4<<CSF)-(2<<CSF));
      break;
    case 152:   // yellow keycard
    case 152+13:
    case 152+13+13:
    case 152+13+13+13:
    case 152+13+13+13+13:
    case 152+13+13+13+13+13:
      give_keycard(DOOR_YELLOW, theplayer, mpx, mpy);
      break;
    case 153:   // red keycard
    case 153+13:
    case 153+13+13:
    case 153+13+13+13:
    case 153+13+13+13+13:
    case 153+13+13+13+13+13:
      give_keycard(DOOR_RED, theplayer, mpx, mpy);
      break;
    case 154:   // green keycard
    case 154+13:
    case 154+13+13:
    case 154+13+13+13:
    case 154+13+13+13+13:
    case 154+13+13+13+13+13:
      give_keycard(DOOR_GREEN, theplayer, mpx, mpy);
      break;
	case 155:   // blue keycard
    case 155+13:
    case 155+13+13:
    case 155+13+13+13:
    case 155+13+13+13+13:
    case 155+13+13+13+13+13:
      give_keycard(DOOR_BLUE, theplayer, mpx, mpy);
      break;
    case DOOR_YELLOW_EP3:
         if (player[theplayer].inventory.HasCardYellow)
           open_door(DOOR_YELLOW, DOOR_YELLOW_SPRITE, mpx, mpy, theplayer);
         break;
    case DOOR_YELLOW_EP3+1:
         if (player[theplayer].inventory.HasCardYellow)
           open_door(DOOR_YELLOW, DOOR_YELLOW_SPRITE, mpx, mpy-1, theplayer);
         break;
    case DOOR_RED_EP3:
         if (player[theplayer].inventory.HasCardRed)
           open_door(DOOR_RED, DOOR_RED_SPRITE, mpx, mpy, theplayer);
         break;
    case DOOR_RED_EP3+1:
         if (player[theplayer].inventory.HasCardRed)
           open_door(DOOR_RED, DOOR_RED_SPRITE, mpx, mpy-1, theplayer);
         break;
    case DOOR_GREEN_EP3:
         if (player[theplayer].inventory.HasCardGreen)
           open_door(DOOR_GREEN, DOOR_GREEN_SPRITE, mpx, mpy, theplayer);
         break;
    case DOOR_GREEN_EP3+1:
         if (player[theplayer].inventory.HasCardGreen)
           open_door(DOOR_GREEN, DOOR_GREEN_SPRITE, mpx, mpy-1, theplayer);
         break;
    case DOOR_BLUE_EP3:
         if (player[theplayer].inventory.HasCardBlue)
           open_door(DOOR_BLUE, DOOR_BLUE_SPRITE, mpx, mpy, theplayer);
         break;
    case DOOR_BLUE_EP3+1:
         if (player[theplayer].inventory.HasCardBlue)
           open_door(DOOR_BLUE, DOOR_BLUE_SPRITE, mpx, mpy-1, theplayer);
         break;

    case 78:
    // cave spikes pointing up
      killplayer(theplayer);
    break;

    // left side of exit door
    case 255:
    case 242:
	  player[theplayer].standingonexit = 1;
      if (can_exit(theplayer))
      {
        PlayerTouchedExit(theplayer, mpx);
      }
      break;
    // middle of exit door (right side of green part)
    case 243:
    case 256:
	  player[theplayer].standingonexit = 1;
    break;

    // we fell off the bottom of the map
    case TILE_FELLOFFMAP_EP3: felloffmap(theplayer); break;

    default: return 1;
   }
   
return 0;
}

// set all players to their start positions
void game_SetStartPositions(void)
{
int x, xdelta, i;

	#define PLAYER_SPACING		(18<<CSF)
	x = map.startx;
	
	if (x>>CSF>>TILE_S > (map.xsize / 2) && levelcontrol.episode != 1)
		xdelta = -PLAYER_SPACING;
	else
		xdelta = PLAYER_SPACING;
		
	for(i=0;i<numplayers;i++)
	{
		player[i].x = x;
		player[i].y = map.starty;
		x += xdelta;
	}
}

// turns the lights on and off
void makedark(int dark)
{
	if (levelcontrol.dark != dark)
	{
		pal_setdark(dark);
		if (!fade_in_progress()) pal_fade(PAL_FADE_SHADES);
		levelcontrol.dark = dark;
	}
}

// called when a player falls off the bottom of the map
void felloffmap(int cp)
{
	if (!player[cp].pdie)
	{
		sound_play(SOUND_KEEN_FALL, PLAY_FORCE);
		player[cp].ankhtime = 0;
		player[cp].godmode = 0;
		player[cp].pdie = PDIE_FELLOFFMAP;
	}
}

// yorp/walker etc "bump".
// if solid = 0, player can possibly force his way through.
// if solid = 1, object acts like a solid "wall".
void bumpplayer(int p, int pushamt, char solid)
{
	player[p].playpushed_x = pushamt;
	
	if (solid)
	{
		if (pushamt > 0)
		{
			if (player[p].pinertia_x < 0)
				player[p].pinertia_x = 0;
		}
		else
		{
			if (player[p].pinertia_x > 0)
				player[p].pinertia_x = 0;
		}
	}
	
	player[p].playpushed_decreasetimer = 0;
	if (!player[p].pjumping)
	{
		player[p].pdir = player[p].pshowdir = (pushamt<0)?LEFT:RIGHT;
	}
}

void scroll_to_player(void)
{
int timeout;
int save;
	
	map_resetscroll();
	// scroll the screen until the primary player is onscreen
	// enough to where he doesn't set off the scroll triggers
	// this is a crappy way of setting the initial scroll position
	save = player[primaryplayer].pdie;
	player[primaryplayer].pdie = 0;
	for(timeout=0;timeout<10000;timeout++)
	{
		if (!gamedo_scrolling(primaryplayer)) break;
	}
	player[primaryplayer].pdie = save;
	
	// scroll past the first two tiles (the level border), they'll
	// now never be visible because you're not allowed to scroll
	// left past X=32.
	while(scroll_x < 32) map_scroll_right();
	while(scroll_y < 32) map_scroll_down();
}

void GiveAnkh(int cp)
{
int o;
	if (!player[cp].ankhtime)
	{
		o = spawn_object(player[cp].x, player[cp].y, OBJ_SECTOREFFECTOR);
		objects[o].ai.se.type = SE_ANKHSHIELD;
		player[cp].ankhshieldobject = o;
	}
	
	sound_play(SOUND_ANKH, PLAY_NOW);
	player[cp].ankhtime = PLAY_ANKH_TIME;
	gamepdo_ankh(cp);
}

