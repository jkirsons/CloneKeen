/* GAMEDO.C
  Contains all of the gamedo_xxx functions...which are called from the
  main game loop. These functions perform some task that is done each
  time around the game loop, not directly related to the player.
*/


#include "keen.h"
#include "gamedo.fdh"

//#define INSTAQUIT			// instant quit on ESC

extern unsigned long gotPlayX;

extern unsigned long CurrentTickCount;

extern unsigned int unknownKey;

int animtiletimer, curanimtileframe;

// gathers data from input controllers: keyboard, joystick, network,
// whatever to populate each player's keytable
// (converts from global keytable[] structure into each player's
// own personal inputs).
void gamedo_getInput(void)
{
int byt;
uchar p;
unsigned int msb, lsb;


	if (levelcontrol.gamemode==GM_DEMOPLAYBACK)
	{
		// time to get a new key block?
		if (!demo.RLERunLen)
		{
			/* get next RLE run length */
			lsb = demo.data[demo.data_index++];
			msb = demo.data[demo.data_index++];
			demo.RLERunLen = (msb<<8) | lsb;
			byt = demo.data[demo.data_index++];         // get keys down
				
			player[0].keytable[PKLEFT] = 0;
			player[0].keytable[PKRIGHT] = 0;
			player[0].keytable[PKJUMP] = 0;
			player[0].keytable[PKPOGO] = 0;
			player[0].keytable[PKFIRE] = 0;
			
			if (byt & DMO_MASK_LEFT) player[0].keytable[PKLEFT] = 1;
			if (byt & DMO_MASK_RIGHT) player[0].keytable[PKRIGHT] = 1;
			if (byt & DMO_MASK_JUMP) player[0].keytable[PKJUMP] = 1;
			if (byt & DMO_MASK_POGO) player[0].keytable[PKPOGO] = 1;
			if (byt & DMO_MASK_FIRE) player[0].keytable[PKFIRE] = 1;
			if (byt & DMO_MASK_END)
			{  // end-of-demo command
				if (!fade_in_progress()) endlevel(WON_LEVEL);
			}
		}
        else
        {
			// we're still in the last RLE run, don't change any keys
			demo.RLERunLen--;
		}
		
        return;
	}
	
	if (Menu_IsVisible()) return;
	
	
	// copy input from keytable into the player inputs
	player[primaryplayer].keytable[PKLEFT] = keytable[KLEFT];
	player[primaryplayer].keytable[PKRIGHT] = keytable[KRIGHT];
	player[primaryplayer].keytable[PKUP] = keytable[KUP];
	player[primaryplayer].keytable[PKDOWN] = keytable[KDOWN];
	player[primaryplayer].keytable[PKSTATUS] = keytable[KENTER];
	
	// NOTE: I realize that if 2-button firing is off, the results of
	// ctrl_alt_demultiplex will be overwritten. But it still allows
	// for eliminating bounce if they hit both keys very quickly, which
	// is a good thing. That's why I run it no matter what.
	ctrl_alt_demultiplex(primaryplayer, keytable[KCTRL], keytable[KALT]);
	if (getoption(OPT_2BUTTONFIRING))
	{
		player[primaryplayer].keytable[PKSTATUS] |= keytable[KSPACE];
	}
	else
	{
		// still need the demultiplexer to eliminate bounce on CTRL+ALT press
		player[primaryplayer].keytable[PKFIRE] = keytable[KSPACE];
	}


    if (numplayers > 1)
    {
		p = primaryplayer ^ 1;
		
		player[p].keytable[PKLEFT] = keytable[KLEFT2];
		player[p].keytable[PKRIGHT] = keytable[KRIGHT2];
		player[p].keytable[PKUP] = keytable[KUP2];
		player[p].keytable[PKDOWN] = keytable[KDOWN2];
		player[p].keytable[PKSTATUS] = keytable[KENTER2];
		
		ctrl_alt_demultiplex(p, keytable[KCTRL2], keytable[KALT2]);
		if (getoption(OPT_2BUTTONFIRING))
		{
			player[p].keytable[PKSTATUS] |= keytable[KSPACE2];
		}
		else
		{
			player[p].keytable[PKFIRE] = keytable[KSPACE2];
		}
    }

	
    if (levelcontrol.gamemode==GM_DEMORECORD)
    {
       __fputc(player[0].keytable[PKLEFT], demo.fp);
       __fputc(player[0].keytable[PKRIGHT], demo.fp);
       __fputc(player[0].keytable[PKJUMP], demo.fp);
       __fputc(player[0].keytable[PKPOGO], demo.fp);
       __fputc(player[0].keytable[PKFIRE], demo.fp);
       __fputc(keytable[KF1], demo.fp);
	   if (keytable[KF1])
	   {
			__fclose(demo.fp); demo.fp = NULL;
			save_compress_demo();
			levelcontrol.gamemode = GM_NORMAL;
			endlevel(QUITTING_PROGRAM);
	   }
    }

}

// given that "ctrl" and "alt" are the current states of those keys,
// turns them into jump, pogo, and fire commands for player cp.
void static ctrl_alt_demultiplex(uchar cp, uchar ctrl, uchar alt)
{
   #define KPROC_IDLE                   0
   #define KPROC_CTRLDEBOUNCE           1
   #define KPROC_ALTDEBOUNCE            2
   #define KPROC_INFIRESTATE            3
   #define KPROC_WAITALLKEYSRELEASED    4
   #define KPROC_WAITCTRLRELEASED       5
   #define KPROC_WAITALTRELEASED        6
   

	switch(player[cp].keyprocstate)
	{
		case KPROC_IDLE:     // idle--waiting for ctrl or alt to be pushed
idlestate: ;
			player[cp].keytable[PKJUMP] = 0;
			player[cp].keytable[PKPOGO] = 0;
			player[cp].keytable[PKFIRE] = 0;
			
			if (alt)
			{
				player[cp].keyprocstate = KPROC_ALTDEBOUNCE;
				player[cp].alttimer = 0;
			}
			else if (ctrl)
			{
				player[cp].keyprocstate = KPROC_CTRLDEBOUNCE;
				player[cp].ctrltimer = 0;
			}
	break;
	
	// ctrl was pressed--give some debounce time to see if player's going
	// for a fire
	case KPROC_CTRLDEBOUNCE:
		if (ctrl && alt)
		{ // he's going for a fire
			player[cp].keyprocstate = KPROC_INFIRESTATE;
			goto firestate;
		}
		else if (!ctrl)
		{ // he let go of ctrl before the debounce time, it's a jump
			player[cp].keytable[PKJUMP] = 1;
			player[cp].keyprocstate = KPROC_WAITCTRLRELEASED;
		}
		else
		{ // ctrl is down, alt is not...keep incing the debounce timer...
			if (player[cp].ctrltimer >= CTRLALT_DEBOUNCETIME)
			{	// time's up, it must have been a jump
				player[cp].keytable[PKJUMP] = 1;
				player[cp].keyprocstate = KPROC_WAITCTRLRELEASED;
			}
			else
			{ // have not reached max debounce time
				player[cp].ctrltimer++;
			}
		}
	break;
	
	// alt was pressed--give some debounce time to see if player's going
	// for a fire
	case KPROC_ALTDEBOUNCE:
		if (ctrl && alt)
		{ // he's going for a fire
			player[cp].keyprocstate = KPROC_INFIRESTATE;
			goto firestate;
		}
		else if (!alt)
		{ // he let go of alt before the debounce time, it's a pogo
			player[cp].keytable[PKPOGO] = 1;
			player[cp].keyprocstate = KPROC_WAITALTRELEASED;
		}
		else
		{ // alt is down, ctrl is not...keep incing the debounce timer...
			if (player[cp].alttimer >= CTRLALT_DEBOUNCETIME)
			{	// time's up, it must have been a pogo
				player[cp].keytable[PKPOGO] = 1;
				player[cp].keyprocstate = KPROC_WAITALTRELEASED;
			}
			else
			{ // have not reached max debounce time
				player[cp].alttimer++;
			}
		}
	break;
	
	case KPROC_INFIRESTATE:
firestate: ;
		// if either CTRL or ALT is released, drop KFIRE and go to
		// WAITALLKEYSRELEASED
		if (ctrl && alt)
		{
			player[cp].keytable[PKFIRE] = 1;
		}
		else
		{
			player[cp].keytable[PKFIRE] = 0;
			player[cp].keyprocstate = KPROC_WAITALLKEYSRELEASED;
		}
	break;
	
	case KPROC_WAITCTRLRELEASED:
		if (!ctrl)
		{
			player[cp].ctrltimer = 0;
			player[cp].alttimer = 0;
			player[cp].keyprocstate = KPROC_IDLE;
			goto idlestate;
		}
		break;
	case KPROC_WAITALTRELEASED:
		if (!alt)
		{
			player[cp].ctrltimer = 0;
			player[cp].alttimer = 0;
			player[cp].keyprocstate = KPROC_IDLE;
			goto idlestate;
		}
	break;
	
	case KPROC_WAITALLKEYSRELEASED:
		// wait for both CTRL and ALT released
		if (!ctrl && !alt)
		{
			player[cp].ctrltimer = 0;
			player[cp].alttimer = 0;
			player[cp].keyprocstate = KPROC_IDLE;
			goto idlestate;
		}
	break;
	}
	   
}

void gamedo_HandleInput(void)
{
int i;
static char lastgodmode = 0, lastctspace = 0;

	// F6 - onscreen debug--toggle through debug/radar/off
	if (keytable[KF6] && !last_keytable[KF6] && getoption(OPT_CHEATS))
	{
		overlay.debugmode ^= 1;
	}
	
	// F10 - fullscreen toggle
	if (keytable[KF10] && !last_keytable[KF10])
	{
		options[OPT_FULLSCREEN] ^= 1;
	}
	
	// no cheats during demo playback
	if (levelcontrol.gamemode != GM_NORMAL)
	{
		keytable[KF2] = 0;
		keytable[KPLUS] = 0;
		keytable[KTAB] = 0;
		return;
	}
	
	if (KeyDrv_KeyIsDown(SDLK_c) && KeyDrv_KeyIsDown(SDLK_t) && KeyDrv_KeyIsDown(SDLK_SPACE))
	{
		if (!lastctspace)
		{
			lastctspace = 1;
			
			Console_Msg("You are now a cheater!");
			for(i=0;i<MAX_PLAYERS;i++)
			{
				if (player[i].isPlaying)
				{
					risebonus(PTCARDY_SPRITE, player[i].x + (1<<CSF), player[i].y - (5<<CSF));
					give_keycard(DOOR_YELLOW, i, 0,0);
					give_keycard(DOOR_RED, i, 0,0);
					give_keycard(DOOR_GREEN, i, 0,0);
					give_keycard(DOOR_BLUE, i, 0,0);
					player[i].inventory.charges = 999;
					player[i].inventory.HasPogo = 1;
					player[i].inventory.lives = 10;
				}
			}
		}
	}
	else
	{
		lastctspace = 0;
		
		// handle calling up the status box
		gamedo_AllowOpenStatusBox();
	}


	// GOD cheat -- toggle god mode
	if (KeyDrv_KeyIsDown(SDLK_g) && KeyDrv_KeyIsDown(SDLK_o) && KeyDrv_KeyIsDown(SDLK_d))
	{
		if (!lastgodmode)
		{
			lastgodmode = 1;
			for(i=0;i<MAX_PLAYERS;i++)
			{
				player[i].godmode ^= 1;
			}
			ClearConsoleMsgs();
			
			if (player[0].godmode)
				Console_Msg("God mode ON");
			else
				Console_Msg("God mode OFF");
			
			sound_play(SOUND_GUN_CLICK, PLAY_FORCE);
			goto revive;
		}
	}
	else lastgodmode = 0;


	// handle F2 - switch primary player
	if(keytable[KF2] && !last_keytable[KF2])
	{
		if (++primaryplayer >= numplayers) primaryplayer = 0;
	}
   

	if (getoption(OPT_CHEATS))		// enables all new cheats besides GOD and C+T+SPACE
    {
            if (keytable[KTAB])		// noclip/revive
            {
revive: ;
			  // resurrect any dead players. the rest of the KTAB magic is
              // scattered throughout the various functions.
              for(i=0;i<MAX_PLAYERS;i++)
              {
                 if (player[i].pdie)
                 {
                   player[i].pdie = PDIE_NODIE;
                   player[i].y -= (8<<CSF);                  
                 }
                 player[i].pfrozentime = 0;
              }
            }
            // F8 - frame by frame
            else if(keytable[KF8] && !last_keytable[KF8])
            {
				if (!framebyframe)
				{
					framebyframe = 1;
					framestorun = 0;
					Console_Msg("Frame-by-frame mode  F8:advance F7:stop");
				}
				else
				{
					framestorun = 1;
				}
            }
			// F7 - stop frame-by-frame mode
            else if(keytable[KF7] && !last_keytable[KF7])
            {
			   if (framebyframe)
			   {
					framebyframe = 0;
					Console_Msg("Frame-by-frame mode: OFF");
			   }
            }
            // F9 - exit level immediately
            else if(keytable[KF9] && !last_keytable[KF9])
            {
               if (!editor) endlevel(WON_LEVEL);
            }
			// F5 - Give some basic necessities
			// (for episode 1 user maps that are borked)
			else if (keytable[KF5] && !last_keytable[KF5])
			{
				sound_play(SOUND_GET_PART, PLAY_NOW);
				Console_Msg("Giving pogo stick and ray gun!");
				for(i=0;i<MAX_PLAYERS;i++)
				{
					if (player[i].isPlaying)
					{
						if (!player[i].inventory.charges)
						{
							player[i].inventory.charges = 2;
						}
						
						player[i].inventory.HasPogo = 1;
					}
				}
			}
    }

    // F3 - change primary player
    if(keytable[KF3] && !last_keytable[KF3])
    {
        if (++primaryplayer >= numplayers) primaryplayer = 0;
    }
}


// handle SPACE/ENTER toggling the status box
void static gamedo_AllowOpenStatusBox(void)
{
	if (cinematic_is_playing()) return;
	
	if ((keytable[KSPACE] && !last_keytable[KSPACE] && getoption(OPT_2BUTTONFIRING)) || \
		(keytable[KENTER] && !last_keytable[KENTER]))
	{
		if (!overlay.statusbox || overlay.statusboxplayer == 1)
		{
			overlay.statusboxplayer = 0;
		}
	}
	else if (numplayers > 1 && ((keytable[KSPACE2] && !last_keytable[KSPACE2] && getoption(OPT_2BUTTONFIRING)) || \
			(keytable[KENTER2] && !last_keytable[KENTER2])))
	{
		if (!overlay.statusbox || overlay.statusboxplayer == 2)
		{
			overlay.statusboxplayer = 1;
		}
	}
	else
	{
		return;
	}
	
	if (!message_up() && !Menu_IsVisible())
	{
		if (!levelcontrol.gameover && !fade_in_progress())
		{
			overlay.statusbox ^= 1;
			if (overlay.statusbox)
				sound_pause();
			else
				sound_resume();
		}
	}
}


// animates animated tiles
void gamedo_AnimatedTiles(void)
{
int i;
	/* animate animated tiles */
	if (animtiletimer > ANIM_TILE_TIME)
	{
      /* advance to next frame */
      curanimtileframe = (curanimtileframe+1) & 7;
      /* re-draw all animated tiles */
      for(i=1;i<MAX_ANIMTILES-1;i++)
      {
			if (animtiles[i].slotinuse)
			{
				sb_drawtile(animtiles[i].x, animtiles[i].y, animtiles[i].baseframe+((animtiles[i].offset+curanimtileframe)%tiles[animtiles[i].baseframe].animlength));
			}
		}
		animtiletimer = 0;
	}
	else animtiletimer++;
}

// compute whether object i is visible and set scrx/scry etc variables
void gamedo_calcenemyvisibility(int i)
{
      objects[i].scrx = (objects[i].x>>CSF)-scroll_x;
      objects[i].scry = (objects[i].y>>CSF)-scroll_y;
      if (objects[i].scrx < -(sprites[objects[i].sprite].xsize) || objects[i].scrx > WINDOW_WIDTH \
          || objects[i].scry < -(sprites[objects[i].sprite].ysize) || objects[i].scry > WINDOW_HEIGHT)
          {
             objects[i].onscreen = 0;
             objects[i].wasoffscreen = 1;
             if (objects[i].type==OBJ_ICEBIT || objects[i].type==OBJ_GOTPOINTS)
				delete_object(i);
          }
          else
          {
             objects[i].onscreen = 1;
             objects[i].hasbeenonscreen = 1;
          }
}

// do object and enemy AI
void gamedo_enemyai(void)
{
int i,topobj;

	// handle objects and do enemy AI
	levelcontrol.numyorps = 0;
	topobj = highest_objslot;		// this is important cause objects might be deleted
	for(i=1;i<topobj;i++)			// can start at 1 cause 0 is always a player
	{
		if (!objects[i].exists || objects[i].type==OBJ_PLAYER) continue;		
		
		gamedo_calcenemyvisibility(i);
		
		if (objects[i].hasbeenonscreen || objects[i].zapped ||
			objects[i].type==OBJ_RAY || \
			objects[i].type==OBJ_ICECHUNK || objects[i].type==OBJ_PLATFORM ||\
			objects[i].type==OBJ_PLATVERT || objects[i].type==OBJ_YORP ||
			objects[i].type==OBJ_FOOB || objects[i].type==OBJ_WALKER)
		{
			common_enemy_ai(i);
			switch(objects[i].type)
			{
				//KEEN1
				case OBJ_YORP: yorp_ai(i); break;
				case OBJ_GARG: garg_ai(i); break;
				case OBJ_VORT: vort_ai(i); break;
				case OBJ_BUTLER: butler_ai(i); break;
				case OBJ_TANK: tank_ai(i); break;
				case OBJ_RAY: ray_ai(i); break;
				case OBJ_DOOR: door_ai(i); break;
				case OBJ_ICECANNON: icecannon_ai(i); break;
				case OBJ_ICECHUNK: icechunk_ai(i); break;
				case OBJ_ICEBIT: icebit_ai(i); break;
				case OBJ_TELEPORTER: teleporter_ai(i); break;
				case OBJ_ROPE: rope_ai(i); break;
				//KEEN2
				case OBJ_WALKER: walker_ai(i); break;
				case OBJ_TANKEP2: tankep2_ai(i); break;
				case OBJ_PLATFORM: platform_ai(i); break;
				case OBJ_VORTELITE: vortelite_ai(i); break;
				case OBJ_SECTOREFFECTOR: se_ai(i); break;
				case OBJ_BABY: baby_ai(i); break;
				case OBJ_EXPLOSION: explosion_ai(i); break;
				case OBJ_EARTHCHUNK: earthchunk_ai(i); break;
				case OBJ_SPARK: spark_ai(i); break;
				//KEEN3
				case OBJ_FOOB: foob_ai(i); break;
				case OBJ_NINJA: ninja_ai(i); break;
				case OBJ_MEEP: meep_ai(i); break;
				case OBJ_SNDWAVE: sndwave_ai(i); break;
				case OBJ_MOTHER: mother_ai(i); break;
				case OBJ_FIREBALL: fireball_ai(i); break;
				case OBJ_BALL: ballandjack_ai(i); break;
				case OBJ_JACK: ballandjack_ai(i); break;
				case OBJ_PLATVERT: platvert_ai(i); break;
				case OBJ_NESSIE: nessie_ai(i); break;
				//Specials
				case OBJ_AUTORAY: case OBJ_AUTORAY_V: autoray_ai(i); break;
				case OBJ_GOTPOINTS: gotpoints_ai(i); break;
				
				default:
					crash("gamedo_enemy_ai: Object %d is of invalid type %d\n", i, objects[i].type);
				break;
			}
			
			objects[i].scrx = (objects[i].x>>CSF) - scroll_x;
			objects[i].scry = (objects[i].y>>CSF) - scroll_y;
		}
	}
}

// common enemy/object ai, such as falling, setting blocked variables,
// detecting player contact, etc.
void common_enemy_ai(int o)
{
int x,y,xa,ya,xsize,ysize;
int temp;
int cplayer;

	if (objects[o].type==OBJ_GOTPOINTS) return;
	
	xsize = sprites[objects[o].sprite].xsize;
	ysize = sprites[objects[o].sprite].ysize;

 // set value of blockedd--should object fall?
	temp = (objects[o].y>>CSF)+ysize;
	if ((temp>>TILE_S)<<TILE_S != temp)
	{
		objects[o].blockedd = 0;
	}
	else
	{ // on a tile boundary, test if tile under object is solid
		objects[o].blockedd = 0;
		x = (objects[o].x>>CSF);
		y = (objects[o].y>>CSF)+ysize+1;
		for(xa=0;xa<xsize-2;xa+=16)
		{
			if (tiles[getmaptileat(x+xa,y)].solidfall || IsStopPoint(x+xa,y,o))
			{
				objects[o].blockedd = 1;
				break;
			}
		}
		
		if (!objects[o].blockedd)	// check final point
		{
			if (tiles[getmaptileat(x+xsize-2, y)].solidfall || IsStopPoint(x+xsize-2,y,o))
			{
				objects[o].blockedd = 1;
			}
		}
	}
	
	// set blockedu
	objects[o].blockedu = 0;
	x = (objects[o].x>>CSF);
	y = (objects[o].y>>CSF)-1;
	for(xa=1;xa<xsize;xa+=16)		// change start pixel to xa=1 for icecannon in ep1l8
	{
		if (tiles[getmaptileat(x+xa,y)].solidceil || IsStopPoint(x+xa,y,o))
		{
			objects[o].blockedu = 1;
			break;
		}
    }

	if (!objects[o].blockedu)		// check final point
	{
		if (tiles[getmaptileat(x+xsize-2, y)].solidceil || IsStopPoint(x+xsize-2,y,o))
		{
			objects[o].blockedu = 1;
		}
	}
	

 // set blockedl
    objects[o].blockedl = 0;
    x = (objects[o].x>>CSF)-1;
    y = (objects[o].y>>CSF)+1;
    for(ya=0;ya<ysize;ya+=16)
    {
        if (tiles[getmaptileat(x,y+ya)].solidr || IsStopPoint(x,y+ya,o))
        {
          objects[o].blockedl = 1;
          goto blockedl_set;
        }
    }
    if (tiles[getmaptileat(x, ((objects[o].y>>CSF)+ysize-1))].solidr || \
		IsStopPoint(x, (objects[o].y>>CSF)+ysize-1, o))
    {
      objects[o].blockedl = 1;
    }
    blockedl_set: ;

 // set blockedr
    objects[o].blockedr = 0;
    x = (objects[o].x>>CSF)+xsize;
    y = (objects[o].y>>CSF)+1;
    for(ya=0;ya<ysize;ya+=16)
    {
        if (tiles[getmaptileat(x,y+ya)].solidl || IsStopPoint(x,y+ya,o))
        {
          objects[o].blockedr = 1;
          goto blockedr_set;
        }
    }
    if (tiles[getmaptileat(x, ((objects[o].y>>CSF)+ysize-1))].solidl || \
		IsStopPoint(x, (x, ((objects[o].y>>CSF)+ysize-1)), o))
    {
      objects[o].blockedr = 1;
    }
    blockedr_set: ;
  
    // hit detection with players
    objects[o].touchPlayer = 0;
    for(cplayer=0;cplayer<MAX_PLAYERS;cplayer++)
    {
      if (player[cplayer].isPlaying)
      {
        objects[player[cplayer].useObject].x = player[cplayer].x;
        objects[player[cplayer].useObject].y = player[cplayer].y;
        objects[player[cplayer].useObject].sprite = 0;
        if (!player[cplayer].pdie)
        {
          if (hitdetect(o, player[cplayer].useObject))
          {
			if (!player[cplayer].godmode)
			{
	            objects[o].touchPlayer = 1;
	            objects[o].touchedBy = cplayer;
			}
			else
			{
				if (objects[o].type==OBJ_MOTHER || objects[o].type==OBJ_BABY ||\
					objects[o].type==OBJ_MEEP || objects[o].type==OBJ_YORP)
				{
					if (objects[o].canbezapped)
						objects[o].zapped += 100;
				}
			}
            break;
          }
        }
      }
    }

// have object fall if it should
  if (!objects[o].inhibitfall)
  {
       #define OBJFALLSPEED   20
       if (objects[o].blockedd)
       {
         objects[o].yinertia = 0;
       }
       else
       {
#define OBJ_YINERTIA_RATE  5
         if (objects[o].yinertiatimer>OBJ_YINERTIA_RATE)
         {
           if (objects[o].yinertia < OBJFALLSPEED) objects[o].yinertia++;
           objects[o].yinertiatimer = 0;
         } else objects[o].yinertiatimer++;
       }
       objects[o].y += objects[o].yinertia;
  }
}



void static update_player_objects(void)
{
int i,o;
   // copy player data to their associated objects so they can get drawn
   // in the object-drawing loop with the rest of the objects
   for(i=0;i<numplayers;i++)
   {
	 o = player[i].useObject;

     if (!player[i].hideplayer)
     {
		objects[o].sprite = player[i].playframe + playerbaseframes[i];
     }
     else
     {
		objects[o].sprite = BLANKSPRITE;
     }

     objects[o].x = player[i].x;
     objects[o].y = player[i].y;
	 // adjust for size difference between regular and die frames
	 if (player[i].pdie)
	 {
		objects[o].y += (sprites[PSTANDFRAME].ysize - sprites[objects[o].sprite].ysize) << CSF;
	 }
   }
}


// returns whether or not object "i" intersect any tiles with
// the priority or masktile flags set
char static object_intersects_priority_tile(int i)
{
int x,y,xsize,ysize;
int ya,xa,tl;

	// in map editor's "stop" mode, never honor priority
	if (editor && !editor_gameisrunning()) return 0;
	
	// get the upper-left coordinates to start checking for tiles
	x = (((objects[i].x>>CSF)-1)>>TILE_S)<<TILE_S;
	y = (((objects[i].y>>CSF)-1)>>TILE_S)<<TILE_S;
	
	// get the xsize/ysize of this sprite, rounded up to the nearest tile size
	xsize = ((sprites[objects[i].sprite].xsize)>>TILE_S<<TILE_S);
	if (xsize != sprites[objects[i].sprite].xsize) xsize+=16;
	
	ysize = ((sprites[objects[i].sprite].ysize)>>TILE_S<<TILE_S);
	if (ysize != sprites[objects[i].sprite].ysize) ysize+=16;
	
	// scan for any priority tiles
	for(ya=0;ya<=ysize;ya+=16)
	{
	  for(xa=0;xa<=xsize;xa+=16)
	  {
		tl = getmaptileat(x+xa,y+ya);
		if (tiles[tl].priority || tiles[tl].masktile)
		{
			return 1;
		}
	  }
	}
	return 0;
}



uchar pmask[MAX_SPRITE_HEIGHT][MAX_SPRITE_WIDTH];
// check if object i is behind a priority tile, and if so, generate
// pmask to show which pixels of the sprite should be drawn
// returns 1 if it intersects a priority tile
char priority_checker(int i)
{
int spr, xsize, ysize;
int x, y, scanx, scany;
int tx, ty, til;

	if (objects[i].honorPriority)
	{
		// figure out if the sprite is going to intersect any priority tiles
		if (object_intersects_priority_tile(i))
		{
			// generate a transparency guide for the sprite that makes
			// the "in front" tiles "obscure" it.
			spr = objects[i].sprite;
			xsize = sprites[spr].xsize;
			ysize = sprites[spr].ysize;
			for(y=0;y<ysize;y++)
			{
				for(x=0;x<xsize;x++)
				{
					if (sprites[spr].maskdata[y][x])
					{
						scanx = (objects[i].x>>CSF) + x;
						scany = (objects[i].y>>CSF) + y;
						til = getmaptileat(scanx, scany);
						if (tiles[til].priority)
						{
							pmask[y][x] = 0;
						}
						else if (tiles[til].masktile)
						{
							// get pix coordinates relative to top of tile
							tx = scanx - ((scanx>>TILE_S)<<TILE_S);
							ty = scany - ((scany>>TILE_S)<<TILE_S);
							pmask[y][x] = (tiledata[tiles[til].masktile][ty][tx]==15)?1:0;
						}
						else pmask[y][x] = 1;
					}
					else pmask[y][x] = 0;
				}
			}
			return 1;
		}
	}
	return 0;
}

void static drawobject_2x(int i)
{
	objects[i].scrx = ((objects[i].x>>CSF)-scroll_x);
	objects[i].scry = ((objects[i].y>>CSF)-scroll_y);
	objects[i].dispx = ((objects[i].x>>(CSF-1))-(scroll_x<<1));
	objects[i].dispy = ((objects[i].y>>(CSF-1))-(scroll_y<<1));
	
	if (priority_checker(i))
	{
		//temphack--make dispx 320x240-accurate only so it doesn't have
		//tiny lines of invisibility sometimes
		drawsprite2x_mask(objects[i].scrx<<1, objects[i].scry<<1, objects[i].sprite, pmask);
	}
	else
	{
		// OBJ_GHOST = ghosted enemy position locators in map editor
		if (objects[i].type == OBJ_GHOST)
		{
		///TEMPHACK should work on 1x too
			drawghostsprite2x(objects[i].dispx, objects[i].dispy, objects[i].sprite);
		}
		else
		{
			drawsprite2x(objects[i].dispx, objects[i].dispy, objects[i].sprite);
		}
	}
}


void static drawobject_1x(int i)
{
	objects[i].scrx = ((objects[i].x>>CSF)-scroll_x);
	objects[i].scry = ((objects[i].y>>CSF)-scroll_y);
	objects[i].dispx = objects[i].scrx;
	objects[i].dispy = objects[i].scry;
	
	if (priority_checker(i))
	{
		drawsprite1x_mask(objects[i].dispx, objects[i].dispy, objects[i].sprite, pmask);
	}
	else
		drawsprite1x(objects[i].dispx, objects[i].dispy, objects[i].sprite);
}


void static Render_drawobjects(void (*draw_object)(int i))
{
int i;
int drawontop[MAX_OBJECTS], num_drawontop = 0;

	// draw all objects. drawn in reverse order because the player sprites
	// are in the first few indexes and we want them to come out on top.
	for(i=highest_objslot-1;i>=0;i--)
	{
		if (objects[i].exists && objects[i].onscreen)
		{
			switch(objects[i].type)
			{
				case OBJ_GOTPOINTS:
					// add it to the drawlist, and draw on top of all other objects
					drawontop[num_drawontop++] = i;
					break;
				default:
					(*draw_object)(i);
					break;
			}	
		}
	}
   
	// ok, now draw everything that we decided to "draw on top"
	for(i=0;i<num_drawontop;i++)
	{
		(*draw_object)(drawontop[i]);
	}
}



void Render_drawdebug(void)
{
int y;
char debugmsg[80];
int savezoom;

	if (overlay.debugmode)
	{
		savezoom = options[OPT_ZOOM];
		options[OPT_ZOOM] = 1;
		
		y = 5-8;
		sprintf(debugmsg, "p1x/y: %d/%d [%d,%d]", player[0].x, player[0].y, player[0].x>>CSF,player[0].y>>CSF);
		font_draw(debugmsg, 5, y+=8, drawcharacter);
		sprintf(debugmsg, "scroll_x/y = %d/%d", (unsigned int)scroll_x, (unsigned int)scroll_y);
		font_draw(debugmsg, 5, y+=8, drawcharacter);
		sprintf(debugmsg, "scrollxy_buf: %d/%d", scrollx_buf, scrolly_buf);
		font_draw(debugmsg, 5, y+=8, drawcharacter);
		sprintf(debugmsg, "playframe: %d", player[0].playframe);
		font_draw(debugmsg, 5, y+=8, drawcharacter);
		sprintf(debugmsg, "puppet: %d", player[0].useObject);
		font_draw(debugmsg, 5, y+=8, drawcharacter);
		sprintf(debugmsg, "highest_objslot: %d", highest_objslot);
		font_draw(debugmsg, 5, y+=8, drawcharacter);
		
		options[OPT_ZOOM] = savezoom;
	}
}



void gamedo_RenderOSD(uchar p)
{
char temp[60];
int x, y;
#define OSD_X		3
#define TEXT_X		(OSD_X+18)
#define SPACING		12
#define OSD_Y		4
	
	// ----------------------
	// = Left side
	// ----------------------
	
	y = OSD_Y;
	if (p)
	{	// adjust Y start positions when multiple players
		if (getoption(OPT_MUSTKILLYORPS))
			y += (50*p);
		else
			y += (35*p);
		
		if (getoption(OPT_SHOWSCORE) && player[p-1].inventory.score > 0)
			y += SPACING;
	}
	
	if (getoption(OPT_SHOWSCORE) && player[p].inventory.score > 0)
	{
		sprintf(temp, "%d", player[p].inventory.score);
		font_draw(temp, OSD_X, y, drawcharacter_clear);
		y += SPACING;
	}
	
	if (player[p].inventory.lives > 0)
	{
		drawsprite(OSD_X, y-1, OSD_LIVES_SPRITE);
		sprintf(temp, "%d", player[p].inventory.lives);
		font_draw(temp, TEXT_X, y, drawcharacter_clear);
		y += SPACING;
	}
	
	if (levelcontrol.curlevel != WORLD_MAP && player[p].inventory.charges > 0)
	{
		drawsprite(OSD_X, y-3, OSD_AMMO_SPRITE);
		if (player[p].inventory.charges >= 999)
			strcpy(temp, "999");
		else
			sprintf(temp, "%d", player[p].inventory.charges);
			
		font_draw(temp, TEXT_X, y, drawcharacter_clear);
		y += SPACING;
	}	
	
	DoMustKillYorps(p, y);
	
	// ----------------------
	// = Right side/keycards
	// ----------------------
	
	// show which keycards the player has
	#define CARDSPACING		(sprites[PTCARDY_SPRITE].ysize + 2)
	x = ((WINDOW_WIDTH - (sprites[PTCARDY_SPRITE].xsize*(p+1))) - OSD_X);
	y = OSD_Y;
	if (getoption(OPT_SHOWCARDS))
	{
		if (player[p].inventory.HasCardYellow)
			{ drawsprite(x, y, PTCARDY_SPRITE); y += CARDSPACING; }
		if (player[p].inventory.HasCardRed)
			{ drawsprite(x, y, PTCARDR_SPRITE); y += CARDSPACING; }
		if (player[p].inventory.HasCardGreen)
			{ drawsprite(x, y, PTCARDG_SPRITE); y += CARDSPACING; }
		if (player[p].inventory.HasCardBlue)
			{ drawsprite(x, y, PTCARDB_SPRITE); y += CARDSPACING; }
	}
	
	// show time left (for timed user maps)
	if (map.hastimelimit && levelcontrol.level_done==LEVEL_NOT_DONE)
	{
		x -= ((5*8) + 4);
		sprintf(temp, "%02d:%02d", levelcontrol.time_left_m, levelcontrol.time_left_s);
		font_draw(temp, x, OSD_Y, drawcharacter_clear);
	}
}


#define YORP_FLASH_SPD		6	// note this speed is RENDER fps relative, not logic fps relative
#define MKY_WAIT_INIT		0
#define MKY_CHECK_STATE		1
#define MKY_NORMAL			2
#define MKY_FLASHDONE		3
#define MKY_FLASHRISE		4
#define MKY_DONE			5
struct
{
	int flash_cur, click;
	int state, timer;
	int lastnumyorps;
	int killedyorptimer;
	int y;
} mky;

void InitMKY(void)
{
	memset(&mky, 0, sizeof(mky));
}

void DoMustKillYorps(int p, int y)
{
int yorpspr, yadj;
char temp[60];

	if (!getoption(OPT_MUSTKILLYORPS)) return;
	if (levelcontrol.curlevel == WORLD_MAP) return;
	if (mky.state==MKY_DONE) return;		// they got all yorps
	
	switch(mky.state)
	{
		// initilize at start of level. first we want to wait a frame
		// for numyorps to become valid. then we want to see if the level
		// even has any yorps to begin with.
		case MKY_WAIT_INIT:
			mky.state = MKY_CHECK_STATE;
			return;
		break;
		
		case MKY_CHECK_STATE:
			if (levelcontrol.numyorps==0)
			{
				mky.state = MKY_DONE;
				return;
			}
			else mky.state = MKY_NORMAL;
		// fall thru
		case MKY_NORMAL:
			mky.y = 0;
			
			// when the player is standing on the exit but he can't leave
			// because there are more yorps to be killed, make this fact
			// clear by flashing the yorps-left number
			if (player[p].standingonexit)
			{
				if (++mky.timer > YORP_FLASH_SPD)
				{
					mky.flash_cur ^= 1;
					mky.timer = 0;
					if (++mky.click > 3)
					{
						sound_play(SOUND_YORP_BUMP, PLAY_NOW);
						mky.click = 0;
					}
				}
			}
			else mky.flash_cur = 0;
			
			// got em all?
			if (levelcontrol.numyorps==0)
			{
				mky.state = MKY_FLASHDONE;
				mky.timer = 0;
				mky.click = 0;
			}
			if (levelcontrol.numyorps < mky.lastnumyorps)
			{
				mky.killedyorptimer = 10;
			}
			mky.lastnumyorps = levelcontrol.numyorps;		
		break;
		
		case MKY_FLASHDONE:
			if (player[p].pdie) return;
			
			if (++mky.timer > YORP_FLASH_SPD/2)
			{
				mky.flash_cur ^= 1;
				mky.timer = 0;
				if (++mky.click%3 == 0)
				{
					sound_play(SOUND_YORP_BUMP, PLAY_NOW);
				}
			}
			
			if (mky.click > (3*6))
			{
				sound_play(SOUND_GET_CARD, PLAY_NOW);
				mky.state = MKY_FLASHRISE;
			}
			else break;
		// fall-thru on done
		
		case MKY_FLASHRISE:
			mky.y -= 3;
			if (mky.y < -50) mky.state = MKY_DONE;
		break;
	}
	
	// draw it to the osd..
	switch(levelcontrol.episode)
	{
		case 1: yorpspr = 51; yadj = -1; break;
		case 2: yorpspr = 111; yadj = 1; break;
		case 3: yorpspr = 95; yadj = 2; break;
	}
	drawsprite(OSD_X, y+yadj+mky.y, yorpspr);
	
	if (levelcontrol.numyorps > 99)
		strcpy(temp, "++");
	else
		sprintf(temp, "%d", levelcontrol.numyorps);
	
	if (mky.state != MKY_FLASHRISE)
	{
		if (mky.flash_cur || mky.killedyorptimer)
			font_draw(temp, TEXT_X, y+mky.y+9, drawcharacter_clear_inversepink);
		else
			font_draw(temp, TEXT_X, y+mky.y+9, drawcharacter_clear);
	}
	
	if (mky.killedyorptimer) mky.killedyorptimer--;
}


// draws sprites, players, and debug messages (if debug mode is on),
// performs frameskipping and blits the display as needed,
// at end of functions erases all drawn objects from the scrollbuf.
extern char palette_dirty;
void gamedo_RenderScreen(void)
{
int x,y,i,bmnum;

	// switch in and out of fullscreen when the option is selected.
	if (options[OPT_FULLSCREEN] != window_is_fullscreen && !editor)
	{
		VidDrv_SetFullscreen(options[OPT_FULLSCREEN]);
	}
	
	if (palette_dirty)
	{
		VidDrv_pal_apply();
		palette_dirty = 0;
	}
	
	// blit scrollbuffer to the display, accounting for wrap etc
	sb_blit();
	
	// draw objects and stuff on top of the terrain
	update_player_objects();
	
	if (!options[OPT_ZOOM])
		Render_drawobjects(drawobject_1x);
	else
		Render_drawobjects(drawobject_2x);
		
	if (!cinematic_is_playing())
	{
		if (overlay.keenslefttime > 0)
		{
			drawkeensleft();
		}
		
		if (getoption(OPT_SHOWOSD) && levelcontrol.gamemode==GM_NORMAL &&\
			!levelcontrol.gameover)
		{
			for(i=0;i<numplayers;i++)
				if (player[i].isPlaying) gamedo_RenderOSD(i);
		}
		
		if (overlay.statusbox)
		{
			DrawStatusBox(overlay.statusboxplayer);
		}
	}
	
	DrawMessages();		// show any storytext
	
	if (levelcontrol.gameover)		// game is over?
	{
		// figure out where to center the gameover bitmap and draw it
		bmnum = GetBitmapNumberFromName("GAMEOVER");
		x = (WINDOW_WIDTH / 2) - (bitmaps[bmnum].xsize / 2);
		y = (WINDOW_HEIGHT / 2) - (bitmaps[bmnum].ysize / 2);
		DrawBitmap(x, y, bmnum);
	}
	
	Render_drawdebug();
	
	// when in main menu, draw title and main menu overlay
	Menu_DrawMenu();
	
	// draw version sprite in corner during demo
	if (levelcontrol.gamemode == GM_DEMOPLAYBACK)
	{
		drawsprite(WINDOW_WIDTH-sprites[VERSION_SPRITE].xsize-2,\
				   WINDOW_HEIGHT-sprites[VERSION_SPRITE].ysize-2,\
				   VERSION_SPRITE);
		
		if (!Menu_IsVisible())
			drawsprite(DEMOBOX_X, DEMOBOX_Y, DEMOBOX_SPRITE);
	}

    DrawConsoleMessages();


	// (this is just a wrapper around SDL_Flip)
	VidDrv_flipbuffer();
}


void gamedo_fades(void)
{
	if (!fade_in_progress()) return;
	
	if (fadecontrol.fadetimer > fadecontrol.rate)
	{
		fadecontrol.fadetimer = 0;
		if (fadecontrol.dir==FADE_IN)
		{			
			if (++fadecontrol.curamt >= PAL_FADE_SHADES)
				fadecontrol.mode = FADE_COMPLETE;
		}
		else if (fadecontrol.dir==FADE_FLASH)
		{
			if (--fadecontrol.curamt <= PAL_FADE_SHADES) 
				fadecontrol.mode = FADE_COMPLETE;
		}
		else if (fadecontrol.dir==FADE_OUT)
		{
			if (fadecontrol.curamt > 0)
			{
				fadecontrol.curamt--;
			}
			else
			{
				fadecontrol.mode = FADE_COMPLETE;
				fadecontrol.fadeout_complete = 1;
			}
		}
		
		pal_fade(fadecontrol.curamt);
	}
    else
	{
		fadecontrol.fadetimer++;
	}
}

// handles scrolling, for player cp, returns nonzero if the scroll was changed
int gamedo_scrolling(int theplayer)
{
signed int px, py;
int scrollchanged;
   if (player[theplayer].pdie) return 0;

   if (levelcontrol.vibratetime)
   {
		do_vibration();		// for episode 2 tantalus ray switch
   }
   
   px = (player[theplayer].x>>CSF)-scroll_x;
   py = (player[theplayer].y>>CSF)-scroll_y;
   scrollchanged = 0;

   /* left-right scrolling */
   if(px > SCROLLTRIGGERRIGHT && scroll_x < max_scroll_x)
   {
      map_scroll_right();
      scrollchanged = 1;
   }
   else if(px < SCROLLTRIGGERLEFT && scroll_x > MIN_SCROLL_X)
   {
      map_scroll_left();
      scrollchanged = 1;
   }

   /* up-down scrolling */
   if (py > SCROLLTRIGGERDOWN && scroll_y < max_scroll_y)
   {
      map_scroll_down();
      scrollchanged = 1;
   }
   else if (py < SCROLLTRIGGERUP && scroll_y > MIN_SCROLL_Y)
   {
      map_scroll_up();
      scrollchanged = 1;
   }

   return scrollchanged;
}

// vibrate, say "oh no", and play "earth blows up" cinematic
void gamedo_TriggerTantalusRay(void)
{
	SetVibrateTime(1790, 1);
}

void gamedo_timedgames(void)
{
int i;

	// handle user maps which must be completed within a certain timeframe
	if (map.hastimelimit && levelcontrol.gamemode==GM_NORMAL &&\
		levelcontrol.level_done==LEVEL_NOT_DONE)
	{
		for(i=0;i<numplayers;i++)
		{
			if (player[i].pdie) return;
		}
		
		if (TimeDrv_HasSecElapsed() || (getoption(OPT_CHEATS) && keytable[KF10]))
		{
			if (levelcontrol.time_left_s)
			{
				levelcontrol.time_left_s--;
			}
			else
			{
				if (levelcontrol.time_left_m)
				{
					levelcontrol.time_left_s = 59;
					levelcontrol.time_left_m--;
				}
				else		// out of time!
				{
					fade(FADE_FLASH, FADE_SLOW);
					// kill all enemies, objects, everything
					for(i=0;i<highest_objslot;i++)
					{
						if (objects[i].exists) killobject(i);
					}
					sound_play(SOUND_VORT_DIE, PLAY_FORCE);
					levelcontrol.died_cause_time_over = 1;
				}
			}
		}
	}
}

// reset the time left to original (for timed games)
void gamedo_resettimelimit(void)
{
	if (map.hastimelimit)
	{
		levelcontrol.time_left_s = map.time_s;
		levelcontrol.time_left_m = map.time_m;
		TimeDrv_ResetSecondsTimer();
	}
}

void gamedo_togglemenus(void)
{
	// ESC toggles menu on and off
	if (keytable[KESC] && !last_keytable[KESC])
	{
		if (Menu_IsVisible())
		{
			Menu_SetVisibility(0);
		}
		else
		{
			#ifdef	INSTAQUIT
				endlevel(QUITTING_PROGRAM);
				fadecontrol.fadeout_complete = 1;	// quit immediately instead of waiting for fade
			#else
				Menu_ToMain();
				Menu_SetVisibility(1);
				sound_play(SOUND_SWITCH_TOGGLE, PLAY_NOW);
			#endif
		}
	}
}


extern uchar gl_retval;
// misc gamelogic run every time around the game loop
// sort of like "glue" between the various gamedo functions
void gamedo_gamelogic(void)
{
int i;
int enter;
int static lastenterstate = 1;

	// gather input and copy to player[].keytable[] structures
	gamedo_getInput();
	gamedo_HandleInput();
	
	Menu_HandleMenu();		// handle menu if it's up
	
	if (framebyframe)
	{
		if (!framestorun)
			goto pausedbottom;
		else
			framestorun--;
	}
	
	gamedo_fades();
	gamedo_AnimatedTiles();
	
	// play cinematic sequences and suspend normal game logic
	// if any are running
	if (cinematics_play()) goto cinematic;
	// things that make the game pause
	if (overlay.statusbox) goto pausedbottom;
	// when loading a savegame, pause until fade-in is complete
	if (levelcontrol.loadgame_freeze)
	{
		if (!Menu_IsVisible())
		{
			if (fade_in_progress()) goto pausedbottom;
							   else levelcontrol.loadgame_freeze = 0;
		}
		else levelcontrol.loadgame_freeze = 0;
	 }	 
	// other things
	if (Menu_IsVisible() && levelcontrol.gameinprogress) goto pausedbottom;
	if (levelcontrol.vibratetime && levelcontrol.vibratepause) goto pausedbottom;
	if (gl_retval==HIT_TANTALUS_SWITCH) goto pausedbottom;
	if (message_up_and_pausing())    // "you see in your mind"
	{
		message_do();
		goto pausedbottom;
	}
	
	// periodically make all enemy gun fixtures fire (in ep3)
	// (also ice cannons in ep1) we do this in a global variable
	// so they're all in sync. when gunfiretimer==0 all gun SE
	// objects will fire.
	if (gunfiretimer > gunfirefreq)
	{
		gunfiretimer = 0;
	}
	else gunfiretimer++;
	
	// handle each player in the game
	if (!map.isworldmap)
	{
		for(i=0;i<MAX_PLAYERS;i++)
		{
			if (player[i].isPlaying) gamepdo_HandlePlayer(i);
		}
	}
	else
	{
		for(i=0;i<MAX_PLAYERS;i++)
		{
			if (player[i].isPlaying) gamepdo_wm_HandlePlayer(i);
		}
	}
	
	gamedo_enemyai();
	gamedo_timedgames();
	
	// when walking through the exit door, don't show keen's sprite past
	// the door frame (so it looks like he walks "through" the door)
	if (levelcontrol.level_done==LEVEL_DONE_WALK)
	{
		gamepdo_walkbehindexitdoor(levelcontrol.level_finished_by);
	}

cinematic: ;
     // allow returning to main menu if it's "game over"
	if (levelcontrol.gameover)
	{
		enter = (keytable[KENTER] || \
				 keytable[KCTRL] || \
				 keytable[KALT] || \
				 keytable[KSPACE]);
		
		if (enter && !lastenterstate)
			endlevel(LOST_LEVEL);
			
		lastenterstate = enter;
	}
	else lastenterstate = 1;


	if (overlay.keenslefttime > 0)
	{
		overlay.keenslefttime--;
	}


pausedbottom: ;
	/* scroll triggers */
	if (!levelcontrol.gameover && \
		levelcontrol.level_done==LEVEL_NOT_DONE && \
		!levelcontrol.dontscroll)
	{
		ScreenIsScrolling = 0;
		if (gamedo_scrolling(primaryplayer)) ScreenIsScrolling = 1;
	}
	
	gamedo_togglemenus();
	
	
	// save key states for next time as the "last" key states
	memcpy(last_keytable, keytable, KEYTABLE_SIZE);
	
}

