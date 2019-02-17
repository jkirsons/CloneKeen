/* GAMEPDO.C
  Contains all of the gamepdo_xxx functions...which are called from the
  main game loop. These functions perform some task that is done each
  time around the game loop and is related to the player.
*/

#include "keen.h"
#include "gamepdo.fdh"

char tempbuf[200];

// player handler mother-function, calls all needed "gamepdo"
// functions for player cp
void gamepdo_HandlePlayer(int cp)
{
char doFall;

	player[cp].standingonexit = 0;
	
    if (player[cp].pdie)
    {
		gamepdo_dieanim(cp);
	}
    else
    {
		if (!levelcontrol.gameover)
		{
			player[cp].inhibitwalking = 0;
			player[cp].inhibitfall = 0;

			gamepdo_exitanim(cp);
			gamepdo_setdir(cp);
			
			gamepdo_setblockedlru(cp);
			gamepdo_getgoodies(cp);
			
			gamepdo_ankh(cp);
			
			gamepdo_raygun(cp);
			
			gamepdo_keencicle(cp);
			
			gamepdo_walking(cp);
			gamepdo_walkinganim(cp);
			
			gamepdo_playpushed(cp);
			gamepdo_InertiaAndFriction_X(cp);
			
			gamepdo_JumpAndPogo(cp);
			
			// decide if player should fall
			doFall = 1;
			if (player[cp].inhibitfall) doFall = 0;
			else if (keytable[KF2] && getoption(OPT_CHEATS)) doFall = 0;
			
			if (doFall)
			{             
				gamepdo_falling(cp);
			}
			else
			{	// fudge that he's standing on something
				player[cp].pfalling = 0;
				player[cp].psupportingtile = 145;	// this is a common floor tile index
				player[cp].psupportingobject = 0;
			}
		}
		else
		{ // we're in game-over mode
		 
		}
    }

    gamepdo_SelectFrame(cp);

    // copy player's keytable to lastkeytable
    memcpy(&player[cp].lastkeytable, &player[cp].keytable, sizeof(player[cp].lastkeytable));
}

void gamepdo_walkbehindexitdoor(int cp)
{
int x, diff, width;

    /* don't draw keen as he walks through the door (past exitXpos) */
    // X pixel position of right side of player
    x = (player[cp].x >> CSF) + PLAYERSPRITE_WIDTH;
    diff = (x - levelcontrol.exitXpos);        // dist between keen and door
    if (diff >= 0)                             // past exitXpos?
    {
       width = (PLAYERSPRITE_WIDTH - diff);    // get new width of sprite
       if (width < 0) width = 0;               // don't set to negative

       // set new width of all player walk frames
       sprites[playerbaseframes[cp]+0].xsize = width;
       sprites[playerbaseframes[cp]+1].xsize = width;
       sprites[playerbaseframes[cp]+2].xsize = width;
       sprites[playerbaseframes[cp]+3].xsize = width;
    }
}


static void lost_level(void) { endlevel(LOST_LEVEL); }
static void timeout_gameover(void)
{
	if (levelcontrol.episode != 2)
	{
		SetGameOver();
	}
	else
	{
		blow_up_world();
	}
}

void gamepdo_dieanim(int cp)
{
	if (!player[cp].pdie) return;                // should never happen...
	if (player[cp].pdie==PDIE_DEAD) return;      // if true animation is over
	if (player[cp].pdie==PDIE_FELLOFFMAP)
	{	// fell off the bottom of map
		// wait for falling sound to complete, then kill the player
		if (!sound_is_playing(SOUND_KEEN_FALL))
		{
			player[cp].pdie = 0;
			killplayer(cp);
		}
		else return;
	}

	// peridocally toggle dying animation frame
	if (player[cp].pdietimer > DIE_ANIM_RATE)
	{
		player[cp].pdieframe = 1 - player[cp].pdieframe;
		player[cp].pdietimer = 0;
	}
	else player[cp].pdietimer++;

	// is it time to start flying off the screen?
	if (!player[cp].pdietillfly)
	{  // fly off the screen
		//if (((player[cp].y>>CSF)+ > scroll_y) && (player[cp].y>(16<<CSF)))
		if (((player[cp].y >> CSF) + sprites[PSTANDFRAME].ysize + 1) > scroll_y)
		{  // player has not reached top of screen
			// make player fly up
			player[cp].y += PDIE_RISE_SPEED;
			if (player[cp].x > (4<<CSF))
			{
				player[cp].x += player[cp].pdie_xvect;
			}
		}
		else
		{  // reached top of screen. he's dead.
			player[cp].pdie = PDIE_DEAD;
			if (player[cp].inventory.lives <= 0)
			{
				player[cp].inventory.lives = -1;
				SetGameOver();
			}
			else
			{
				whodied = cp;
				if (!editor && levelcontrol.died_cause_time_over)
				{
					dispmsgstring("TimeOverString", 1);
					
					if (!map.GameOverOnTimeOut)
						message_SetDismissalCallback(lost_level);
					else
						message_SetDismissalCallback(timeout_gameover);
				}
				else
				{
					lost_level();
				}
			}
		}
	}
	else
	{  // not yet time to fly off screen, decrement timer
		player[cp].pdietillfly--;
	}  // end "time to fly"

}

void gamepdo_keencicle(int cp)
{
   // keencicle code (when keen gets hit by an ice chunk)
   if (player[cp].pfrozentime)
   {
     if (player[cp].pfrozentime > PFROZEN_THAW)
     {
        if (player[cp].pfrozenanimtimer > PFROZENANIMTIME)
        {
          if (player[cp].pfrozenframe) player[cp].pfrozenframe=0; else player[cp].pfrozenframe=1;
          player[cp].pfrozenanimtimer = 0;
        }
        else player[cp].pfrozenanimtimer++;
     }
     else
     { // thawing out, show the thaw frame
        if (levelcontrol.episode==3)
          player[cp].pfrozenframe = 2;
        else
          player[cp].pfrozenframe = 3;
     }

     player[cp].pfrozentime--;
     player[cp].inhibitwalking = 1;
   }

}

// forces the RIGHT key if we're doing the walk-out-the-exit thing
void gamepdo_exitanim(int cp)
{   
   if (!levelcontrol.level_done || levelcontrol.level_finished_by!=cp) return;

	 // don't let player control keen
     player[cp].keytable[PKJUMP] = 0;
     player[cp].keytable[PKPOGO] = 0;
     player[cp].keytable[PKFIRE] = 0;
     player[cp].keytable[PKLEFT] = 0;
     //player[cp].inhibitfall = 1;
     if (levelcontrol.level_done==LEVEL_DONE_WALK)
     {
        // keep him going right
        player[cp].pdir = player[cp].pshowdir = RIGHT;
        // make keen walk slowly through the exit door
        player[cp].keytable[PKRIGHT] = 1;
        if (player[cp].pinertia_x > PMAXEXITDOORSPEED)
        {
          player[cp].pinertia_x = PMAXEXITDOORSPEED;
        }
     }
     else
     {
        // he's all the way through the door; hold him still
        player[cp].keytable[PKRIGHT] = 0;
        player[cp].pinertia_x = 0;
        player[cp].pwalking = 0;
     }

     if (levelcontrol.level_done_timer > LEVEL_DONE_TOTAL_WAIT_TIME)
     {
       if (levelcontrol.level_done != LEVEL_DONE_FADEOUT)
       {
			if (!editor)
			{
				levelcontrol.level_done = LEVEL_DONE_FADEOUT;
				endlevel(WON_LEVEL);
			}
			else
			{
				levelcontrol.level_done = LEVEL_NOT_DONE;
				editor_wonlevel();
			}
       }
     }
     else if (levelcontrol.level_done_timer > LEVEL_DONE_STOPWALKING_TIME)
     {
       levelcontrol.level_done = LEVEL_DONE_WAIT;
     }

     levelcontrol.level_done_timer++;
     return;
}

// if player not sliding and not jumping, allow
// them to change their direction. if jumping,
// we can change direction but it will not be shown
// in the frame.
void gamepdo_setdir(int cp)
{
char stuck;

   if (player[cp].pfrozentime) return;
   // can't change direction on ice,
   // UNLESS we're stuck up against a wall
   if (player[cp].psliding)
   {
     stuck = 0;
     if (player[cp].pshowdir == LEFT && player[cp].blockedl) stuck = 1;
     else if (player[cp].pshowdir == RIGHT && player[cp].blockedr) stuck = 1;
     if (stuck)
     {
       // jumped off an ice block into a wall?
       if (player[cp].pjumping || player[cp].pfalling)
       {
         player[cp].psliding = 0;
       }       
     }
     else
     {
       // since we're not stuck up against a wall, we can't change direction
       return;
     }
   }
  
   if (!player[cp].pjumping && !player[cp].pfiring)
   {
     if (player[cp].keytable[PKLEFT]) { player[cp].pdir = player[cp].pshowdir = LEFT; }
     if (player[cp].keytable[PKRIGHT]) { player[cp].pdir = player[cp].pshowdir = RIGHT; }
   }
   else
   {
     if (player[cp].keytable[PKLEFT]) { player[cp].pdir = LEFT; }
     if (player[cp].keytable[PKRIGHT]) { player[cp].pdir = RIGHT; }
   }
}

// set blockedl/r/u...is Keen up against a solid object?
void gamepdo_setblockedlru(int cp)
{
int tx,ty;
   player[cp].blockedl = player[cp].blockedr = 0;
   player[cp].blockedu = 0;
   if (levelcontrol.level_done && levelcontrol.level_finished_by==cp) return;

   if ((player[cp].x>>CSF) < 2) player[cp].blockedl = 1;

   // this "if" is for no-clipping
   if (!getoption(OPT_CHEATS) || (!keytable[KTAB] && !keytable[KF2]))
   {
	// set blockedu
      tx = (player[cp].x>>CSF)+4;
      ty = (player[cp].y>>CSF);
      if (tiles[getmaptileat(tx,ty)].solidceil || checkobjsolid(tx<<CSF,ty<<CSF,cp))
      {
          player[cp].blockedu = 1;
          if (tiles[getmaptileat(tx,ty)].bonklethal) killplayer(cp);
      }
      else
      {
         tx = (player[cp].x>>CSF)+12;
         if (tiles[getmaptileat(tx,ty)].solidceil || checkobjsolid(tx<<CSF,ty<<CSF,cp))
         {
            player[cp].blockedu = 1;
            if (tiles[getmaptileat(tx,ty)].bonklethal) killplayer(cp);
         }
      }

       // we use checkissolidl to check for blockedr, and vice versa
       // for blockedl. this is because here we are checking for the
       // right of the player--which will be the left of a potential tile.
      if (checkissolidl((player[cp].x>>CSF)+13, (player[cp].y>>CSF)+1,cp))
         { player[cp].blockedr = 1; }
      else if (checkissolidl((player[cp].x>>CSF)+13, (player[cp].y>>CSF)+8,cp))
         { player[cp].blockedr = 1; }
      else if (checkissolidl((player[cp].x>>CSF)+13, (player[cp].y>>CSF)+16,cp))
         { player[cp].blockedr = 1; }
      else if (checkissolidl((player[cp].x>>CSF)+13, (player[cp].y>>CSF)+(sprites[0].ysize-1),cp))
         { player[cp].blockedr = 1; }

      // for one-way force fields in ep3, so you can back out if you're
      // not all the way through yet
      if (player[cp].blockedr && tiles[player[cp].blockedby].solidl && !tiles[player[cp].blockedby].solidr)
      {
        if (((player[cp].x>>CSF)+13)>>4<<4 != ((player[cp].x>>CSF)+13))
        {  // not on a tile boundary.
          player[cp].blockedr = 0;
        }
      }
        
      if (checkissolidr((player[cp].x>>CSF)+2, (player[cp].y>>CSF)+1,cp))
         { player[cp].blockedl = 1; }
      else if (checkissolidr((player[cp].x>>CSF)+2, (player[cp].y>>CSF)+8,cp))
         { player[cp].blockedl = 1; }
      else if (checkissolidr((player[cp].x>>CSF)+2, (player[cp].y>>CSF)+16,cp))
         { player[cp].blockedl = 1; }
      else if (checkissolidr((player[cp].x>>CSF)+2, (player[cp].y>>CSF)+(sprites[0].ysize-1),cp))
         { player[cp].blockedl = 1; }

      if (player[cp].blockedl && tiles[player[cp].blockedby].solidr && !tiles[player[cp].blockedby].solidl)
      {
        if ((((player[cp].x>>CSF)+2)>>4<<4)+15 != ((player[cp].x>>CSF)+2))
        {  // not on a tile boundary.
          player[cp].blockedl = 0;
        }
      }

   }
   else player[cp].playpushed_x = 0;
}

// let's have keen be able to pick up goodies
void gamepdo_getgoodies(int cp)
{
   if (tiles[getmaptileat((player[cp].x>>CSF)+9, (player[cp].y>>CSF)+1)].goodie)
      { keen_get_goodie((player[cp].x>>CSF)+9, (player[cp].y>>CSF)+1, cp); return; }
   else if (tiles[getmaptileat((player[cp].x>>CSF)+4, (player[cp].y>>CSF)+8)].goodie)
      { keen_get_goodie((player[cp].x>>CSF)+4, (player[cp].y>>CSF)+8, cp); return; }
   else if (tiles[getmaptileat((player[cp].x>>CSF)+9, (player[cp].y>>CSF)+16)].goodie)
      { keen_get_goodie((player[cp].x>>CSF)+9, (player[cp].y>>CSF)+16, cp); return; }
   else if (tiles[getmaptileat((player[cp].x>>CSF)+4, (player[cp].y>>CSF)+23)].goodie)
      { keen_get_goodie((player[cp].x>>CSF)+4, (player[cp].y>>CSF)+23, cp); }
}

// animation for walking
void gamepdo_walkinganim(int cp)
{
    // no walk animation while sliding
    if (player[cp].inhibitwalking || player[cp].psliding) return;

    // should we do walk animation?
    if (player[cp].pwalking || player[cp].playpushed_x)
    {
        // ** do walk animation **
        if (player[cp].pwalkanimtimer > PWALKANIMRATE)
        { // time to change walking frame
          // make walk noise
          if (!player[cp].pjumping && !player[cp].pfalling)
          {
            if (!player[cp].pfrozentime)
            {
               if (player[cp].pwalkframea&1)
                 { sound_play(SOUND_KEEN_WALK, PLAY_NOW); }
               else
                 { sound_play(SOUND_KEEN_WALK2, PLAY_NOW); }
            }
          }
          // increase walk frame and wrap it to 1st frame if needed
          if (player[cp].pwalkframea < 4)
            { player[cp].pwalkframea++; }
          else
            { player[cp].pwalkframea=1; }

          player[cp].pwalkanimtimer = 0;
        }
        else
        {  // did not change walk frame
          player[cp].pwalkanimtimer++;
        }

        // set walk frame: map frame "4" to frame "2", this gives a
        // sequence of 1,2,3,2,1,2,3,2,1,2,3,2....
        if (player[cp].pwalkframea==4)
        {
          player[cp].pwalkframe = 2;
        }
        else
        {
          player[cp].pwalkframe = player[cp].pwalkframea;
        }
    }
}

// handle playpushed_x: for yorps/walkers/etc pushing keen
void gamepdo_playpushed(int cp)
{
    if (getoption(OPT_CHEATS) && keytable[KTAB]) return;

    // if we're being pushed...
    if (player[cp].playpushed_x)
    {
      // do friction on push force...
      if (player[cp].playpushed_decreasetimer>PLAYPUSH_DECREASERATE)
      {
        // push playpushed_x towards zero
        if (player[cp].playpushed_x < 0)
        {
          player[cp].playpushed_x++;
        }
        else
        {
          player[cp].playpushed_x--;
        }
        player[cp].playpushed_decreasetimer = 0;
      }
      else player[cp].playpushed_decreasetimer++;

      // if we run up against a wall all push inertia stops
      if (player[cp].playpushed_x > 0 && player[cp].blockedr) player[cp].playpushed_x = 0;
      if (player[cp].playpushed_x < 0 && player[cp].blockedl) player[cp].playpushed_x = 0;
    }

}

// handles inertia and friction for the X direction
// (this is where the inertia/playpushed_x is actually applied to playx)
void gamepdo_InertiaAndFriction_X(int cp)
{
int friction_rate;

   if (player[cp].hideplayer)
   {
     player[cp].pinertia_x = 0;
     return;
   }
   // don't move when firing
   if (player[cp].pfiring && !player[cp].pjumping && !player[cp].pfalling)
   {
	 player[cp].pinertia_x = 0;
	 return;
   }

   // if we hit a solid object do not move, and keep inertia...
   // * at slow speed: if we're falling or jumping and we're facing
   //                  the right direction, we want to keep
   //                  a small amount of inertia pent up so we can
   //                  easily get into tight spaces.
   // * at zero:       otherwise, or if we're not walking, we want
   //                  to hold the inertia at zero so we don't "shoot"
   //                  off of platforms, etc which looks weird.
   if (!levelcontrol.level_done || levelcontrol.level_finished_by!=cp)
   {
     if (player[cp].pinertia_x < 0 && player[cp].blockedl)
     {
       if (!player[cp].pwalking || (!player[cp].pfalling && !player[cp].pjumping) || player[cp].pdir==RIGHT || (player[cp].pfrozentime&&levelcontrol.episode==1)) player[cp].pinertia_x = 0;
       else if (player[cp].pinertia_x < -PFASTINCMAXSPEED) player[cp].pinertia_x = -PFASTINCMAXSPEED;
       return;
     }
     else if (player[cp].pinertia_x > 0 && player[cp].blockedr)
     {
       if (!player[cp].pwalking || (!player[cp].pfalling && !player[cp].pjumping) || player[cp].pdir==LEFT || (player[cp].pfrozentime&&levelcontrol.episode==1)) player[cp].pinertia_x = 0;
       else if (player[cp].pinertia_x > PFASTINCMAXSPEED) player[cp].pinertia_x = PFASTINCMAXSPEED;
       return;
     }
   }

   // apply pinertia_x and playpushed_x inertia
   // (unless we're about to make a pogo jump)
   if (player[cp].pjumping != PPREPAREPOGO)
   {
     player[cp].x += (player[cp].pinertia_x + player[cp].playpushed_x);
   }

   // if we stopped walking (i.e. left or right not held down) apply friction
   // there's no friction if we're semisliding
   if (!player[cp].keytable[PKLEFT] && !player[cp].keytable[PKRIGHT] && \
       !player[cp].psemisliding)
   {
     // determine friction rate--different rates for on ground and in air
     if (map.isworldmap)
     {
       friction_rate = PFRICTION_RATE_WM;
     }
     else
     {
        if (!player[cp].pfalling & !player[cp].pjumping)
        {
          friction_rate = PFRICTION_RATE_ONGROUND;
        }
        else
        {
          friction_rate = PFRICTION_RATE_INAIR;
        }
     }

     // and apply friction to pinertia_x
     // when pogoing apply friction till we get down to PFASTINCMAXSPEED
     // then stop the friction
     if (!player[cp].ppogostick || (player[cp].pinertia_x > PFASTINCMAXSPEED) || (player[cp].pinertia_x < -PFASTINCMAXSPEED))
     {
        if (player[cp].pfriction_timer_x > friction_rate)
        {
          if (!player[cp].pfrozentime || levelcontrol.episode!=1)
          {  // disable friction while frozen
             if (player[cp].pinertia_x < 0) player[cp].pinertia_x++;
             else if (player[cp].pinertia_x > 0) player[cp].pinertia_x--;
          }

          player[cp].pfriction_timer_x = 0;
        }
        else player[cp].pfriction_timer_x++;
     }
   }

}

// called when a switch is flipped. mx,my is the pixel coords of the switch,
// relative to the upper-left corner of the map.
void gamepdo_ExtendingPlatformSwitch(int x, int y)
{
uint ppos;
int platx, platy;
signed char pxoff, pyoff;
int mapx, mapy;
int o;

	// convert pixel coords to tile coords
	mapx = (x >> TILE_S);
	mapy = (y >> TILE_S);

	// figure out where the platform is supposed to extend at
	// (this is coded in the object layer...
	// high byte is the Y offset and the low byte is the X offset,
	// and it's relative to the position of the switch.)
	ppos = getlevelat(x, y);
	
	if (!ppos || !levelcontrol.PlatExtending)
	{
		// flip switch
		sound_play(SOUND_SWITCH_TOGGLE, PLAY_NOW);
		if (getmaptileat(x, y)==TILE_SWITCH_DOWN)
			map_chgtile(mapx, mapy, TILE_SWITCH_UP);
		else
			map_chgtile(mapx, mapy, TILE_SWITCH_DOWN);
	}
	
	// if zero it means he hit the switch on a tantalus ray!
	if (!ppos)
	{
		if (editor)	  // in editor it'll bork if we play a cinematic, so don't
		{
			killplayer(0);
		}
		else
		{
			gamedo_TriggerTantalusRay();
		}
		return;
	}
	else
	{
		// it's a moving platform switch--don't allow player to hit it again while
		// the plat is still moving as this will glitch
		if (levelcontrol.PlatExtending) return;
		levelcontrol.PlatExtending = 1;
	}

	pxoff = (ppos & 0x00ff);
	pyoff = (ppos & 0xff00) >> 8;
	platx = mapx + pxoff;
	platy = mapy + pyoff;

	// spawn a "sector effector" to extend/retract the platform
	o = spawn_object(mapx<<TILE_S<<CSF,mapy<<TILE_S<<CSF,OBJ_SECTOREFFECTOR);
	objects[o].ai.se.type = SE_EXTEND_PLATFORM;
	objects[o].ai.se.platx = platx;
	objects[o].ai.se.platy = platy;
}

// allow Keen to toggle the pogo stick and hit switches
void gamepdo_TogglePogo_and_Switches(int cp)
{
int i;
int mx, my, t;

	// detect if KPOGO key just pressed
	if (player[cp].keytable[PKPOGO] && !player[cp].lastkeytable[PKPOGO] && !player[cp].pfrozentime)
	{
		// if we are standing near a switch hit the switch instead
		mx = (player[cp].x>>CSF)+(sprites[PSTANDFRAME].xsize/2);

		for(i=sprites[PSTANDFRAME].ysize-1;i>=0;i-=8)
		{
			my = (player[cp].y>>CSF)+i;

			t = getmaptileat(mx, my);

			// check for extending-platform switch
			if ((t==TILE_SWITCH_UP || t==TILE_SWITCH_DOWN) && \
				levelcontrol.episode==2)
			{
				gamepdo_ExtendingPlatformSwitch(mx, my);
				if (!player[cp].ppogostick) return;
			}
			else if (t==TILE_LIGHTSWITCH)
			{ // lightswitch
				makedark(levelcontrol.dark ^ 1);
				sound_play(SOUND_SWITCH_TOGGLE, PLAY_NOW);
				if (!player[cp].ppogostick) return;
			}
		}
   
		// toggle pogo stick
		if (player[cp].inventory.HasPogo)
		{
			player[cp].ppogostick ^= 1;
		}
	}
}

void gamepdo_JumpAndPogo(int cp)
{

	// allow him to toggle the pogo stick
	gamepdo_TogglePogo_and_Switches(cp);

   // handle the JUMP key, both for normal jumps and (high) pogo jumps
   if (!player[cp].pjumping && !player[cp].pfalling && !player[cp].pfiring)
   {
     // give em the chance to jump
     if (player[cp].keytable[PKJUMP] && !player[cp].ppogostick && !player[cp].pfrozentime)
     {
       player[cp].pjumping = PPREPAREJUMP;
       player[cp].pjumpframe = PPREPAREJUMPFRAME;
       player[cp].pjumpanimtimer = 0;
       player[cp].pwalking = 0;
     }
     else if (player[cp].ppogostick)
     {
       player[cp].pjumping = PPREPAREPOGO;
       player[cp].pjumpanimtimer = 0;
       player[cp].pwalking = 0;
     }
   }

    switch(player[cp].pjumping)
    {
      case PPREPAREPOGO:
          if (player[cp].pjumpanimtimer > PPOGO_PREPARE_TIME)
          {
             // continously bounce while pogo stick is out
             sound_play(SOUND_KEEN_JUMP, PLAY_NOW);
             // jump high if JUMP key down, else bounce low
             if (player[cp].keytable[PKJUMP])
             {
               if (!getoption(OPT_SUPERPOGO))
               {  // normal high pogo jump
                  player[cp].pjumpupspeed = PPOGO_HIGHJUMP_SPEED;
                  player[cp].pjumptime = PJUMP_NORMALTIME_POGO_LONG;
                  player[cp].pjumpupdecreaserate = PJUMP_UPDECREASERATE_POGO_LONG;
               }
               else
               {
                  player[cp].pjumpupspeed = PPOGO_SUPERHIGHJUMP_SPEED;
                  player[cp].pjumptime = PJUMP_NORMALTIME_POGO_LONG_SUPER;
                  player[cp].pjumpupdecreaserate = PJUMP_UPDECREASERATE_POGO_LONG_SUPER;                 
               }
             }
             else		// bouncing on pogo
             {
               player[cp].pjumpupspeed = PPOGO_LOWJUMP_SPEED;
               player[cp].pjumptime = PJUMP_NORMALTIME_POGO_SHORT;
               player[cp].pjumpupdecreaserate = PJUMP_UPDECREASERATE_POGO_SHORT;
             }
             player[cp].pjumpframe = PJUMP_PREPARE_LAST_FRAME;
             player[cp].pjumping = PPOGOING;
             player[cp].pjumpupspeed_decreasetimer = 0;
             player[cp].pjustjumped = 1;
             if (player[cp].keytable[PKLEFT])
                { player[cp].pinertia_x = -PMAXSPEED; player[cp].pdir = player[cp].pshowdir = LEFT; }
             if (player[cp].keytable[PKRIGHT])
                { player[cp].pinertia_x = PMAXSPEED; player[cp].pdir = player[cp].pshowdir = RIGHT; }
          } else player[cp].pjumpanimtimer++;
          break;
      case PPREPAREJUMP:
             player[cp].pinertia_x = 0;     // prevent moving while preparing to jump
             if (player[cp].pjumpanimtimer > PJUMP_PREPARE_ANIM_RATE)
             {
                  if (player[cp].pjumpframe == PJUMP_PREPARE_LAST_FRAME || !player[cp].keytable[PKJUMP])
                  {  // time to start the jump
                  // select a jump depending on how long keen was preparing
                       player[cp].pjumpupspeed = PJUMPUP_SPEED;
                       switch(player[cp].pjumpframe)
                       {
                       case PPREPAREJUMPFRAME:
                            player[cp].pjumptime = PJUMP_NORMALTIME_6;
                            player[cp].pjumpupdecreaserate = PJUMP_UPDECREASERATE_6;
                            player[cp].pjumpupspeed = 7;
                            break;
                       case PPREPAREJUMPFRAME+1:
                            player[cp].pjumptime = PJUMP_NORMALTIME_5;
                            player[cp].pjumpupdecreaserate = PJUMP_UPDECREASERATE_5;
                            player[cp].pjumpupspeed = 10;
                            break;
                       case PPREPAREJUMPFRAME+2:
                            player[cp].pjumptime = PJUMP_NORMALTIME_4;
                            player[cp].pjumpupdecreaserate = PJUMP_UPDECREASERATE_4;
                            break;
                       case PPREPAREJUMPFRAME+3:
                            player[cp].pjumptime = PJUMP_NORMALTIME_3;
                            player[cp].pjumpupdecreaserate = PJUMP_UPDECREASERATE_3;
                            break;
                       case PPREPAREJUMPFRAME+4:
                            player[cp].pjumptime = PJUMP_NORMALTIME_2;
                            player[cp].pjumpupdecreaserate = PJUMP_UPDECREASERATE_2;
                            break;
                       default:
                            player[cp].pjumptime = PJUMP_NORMALTIME_1;
                            player[cp].pjumpupdecreaserate = PJUMP_UPDECREASERATE_1;
                            break;
                       }
                    player[cp].pjumpframe = PJUMP_PREPARE_LAST_FRAME;

                    sound_play(SOUND_KEEN_JUMP, PLAY_NOW);
                    player[cp].pjumping = PJUMPUP;
                    player[cp].pjumpupspeed_decreasetimer = 0;
                    player[cp].pjustjumped = 1;
                    player[cp].pjumpfloattimer = 0;

                    // make so if we're jumping left or right
                    // the walk code will start at full speed
                    player[cp].pwalking = 1;
                    player[cp].pwalkanimtimer = 0;
                    player[cp].pwalkframe = 1;
                    if (tiles[player[cp].psupportingtile].ice)
                    { // on ice, always jump direction facing
					  player[cp].pdir = player[cp].pshowdir;
                      if (player[cp].pshowdir==LEFT)
                        { player[cp].pinertia_x = -PMAXSPEED; }
                      else
                        { player[cp].pinertia_x = PMAXSPEED; }
                    }
                    else
                    {
                      if (player[cp].keytable[PKLEFT])
                        { player[cp].pinertia_x = -PMAXSPEED; player[cp].pdir = player[cp].pshowdir = LEFT; }
                      if (player[cp].keytable[PKRIGHT])
                        { player[cp].pinertia_x = PMAXSPEED; player[cp].pdir = player[cp].pshowdir = RIGHT; }
                    }
                    player[cp].pwalkincreasetimer = 0;
                  }
                  else
                  {
                    player[cp].pjumpframe++;
                  }
                  player[cp].pjumpanimtimer=0;
             } else player[cp].pjumpanimtimer++;
             break;
        case PJUMPUP:
        case PPOGOING:
        // check for hitting a ceiling
         if (player[cp].blockedu)   // did we bonk something?
         {  // immediatly abort the jump
            player[cp].pjumping = PNOJUMP;
            sound_play(SOUND_KEEN_BUMPHEAD, PLAY_NOW);
         }
         // do the jump
         if (!player[cp].pjumptime)
         {
           if (player[cp].pjumpupspeed_decreasetimer>player[cp].pjumpupdecreaserate)
           {
              if (!player[cp].pjumpupspeed)
              {
                player[cp].pjumping = PNOJUMP;
              } else player[cp].pjumpupspeed--;
              player[cp].pjumpupspeed_decreasetimer=0;
           } else player[cp].pjumpupspeed_decreasetimer++;
         }
         else player[cp].pjumptime--;
         player[cp].y -= player[cp].pjumpupspeed;
    break;
    }

}

void gamepdo_falling(int cp)
{
unsigned int temp;
int objsupport, tilsupport;

    player[cp].pfalling = 0;         // assume not falling

    // do not fall if we're jumping
    if (player[cp].pjumping)
    {
      player[cp].psemisliding = 0;
      return;
    }

    // ** determine if player should fall (nothing solid is beneath him) **

    player[cp].psupportingtile = BG_GREY;
    player[cp].psupportingobject = 0;
    // test if tile under player is solid; if so set psupportingtile
    objsupport = checkobjsolid(player[cp].x+(4<<CSF), player[cp].y+(sprites[0].ysize<<CSF),cp);
    tilsupport = checkissolidd(getmaptileat((player[cp].x>>CSF)+4, (player[cp].y>>CSF)+sprites[0].ysize), cp);
    if (!tilsupport && !objsupport)
    { // lower-left isn't solid
		objsupport = checkobjsolid(player[cp].x+(12<<CSF), player[cp].y+(sprites[0].ysize<<CSF),cp);
		tilsupport = checkissolidd(getmaptileat((player[cp].x>>CSF)+12, (player[cp].y>>CSF)+sprites[0].ysize), cp);
		if (!tilsupport && !objsupport)
		{  // lower-right isn't solid
			player[cp].pfalling = 1;        // so fall.
			player[cp].pjustfell = 1;
		}
		else
		{  // lower-left isn't solid but lower-right is
			if (!objsupport)
			{
				player[cp].psupportingtile = getmaptileat((player[cp].x>>CSF)+12, (player[cp].y>>CSF)+sprites[0].ysize);
				// spikes that hurt you when you stand on them
				if (tiles[player[cp].psupportingtile].standgoodie)
				{
					keen_get_goodie((player[cp].x>>CSF)+12, (player[cp].y>>CSF)+sprites[0].ysize, cp);
				}
			}
			else
			{
				player[cp].psupportingtile = PSUPPORTEDBYOBJECT;
				player[cp].psupportingobject = objsupport;
			}
		}
    }
    else
    {   // lower-left is solid
        if (!objsupport)
        {
			player[cp].psupportingtile = getmaptileat((player[cp].x>>CSF)+4, (player[cp].y>>CSF)+sprites[0].ysize);
			if (tiles[player[cp].psupportingtile].standgoodie)
			{
				keen_get_goodie((player[cp].x>>CSF)+4, (player[cp].y>>CSF)+sprites[0].ysize, cp);
			}
		}
		else
		{
			player[cp].psupportingtile = PSUPPORTEDBYOBJECT;
			player[cp].psupportingobject = objsupport;
		}
    }

    // if not on a tile boundary, always fall; prevents being able
    // to land in the middle of a tile.
    if (!player[cp].pfalling && player[cp].psupportingtile != PSUPPORTEDBYOBJECT)
    {
		temp = (player[cp].y>>CSF)+sprites[0].ysize;    // bottom of player
		if ((temp>>4)<<4 != temp)   // true if it's not a multiple of 16
		{
			player[cp].pfalling = 1;   // not on a tile boundary. fall.
			player[cp].pjustfell = 1;
			player[cp].psupportingtile = BG_GREY;
			player[cp].psupportingobject = 0;
		}
    }
	
    // if supported by an object make sure we're at the top of
    // the object, else fall
    if (!player[cp].pfalling && player[cp].psupportingtile==PSUPPORTEDBYOBJECT)
    {
		if ((player[cp].y>>CSF)+sprites[0].ysize > (objects[player[cp].psupportingobject].y>>CSF)+4)
		{
			if (!tilsupport)
			{
				player[cp].pfalling = 1;
				player[cp].pjustfell = 1;
				player[cp].psupportingtile = BG_GREY;
				player[cp].psupportingobject = 0;
			}
		}
    }

    // the first time we land on an object, line us up to be exactly on
    // top of the object
    if (player[cp].psupportingobject && !player[cp].lastsupportingobject)
    {
		player[cp].y = objects[player[cp].psupportingobject].y - (sprites[0].ysize<<CSF);
    }
    player[cp].lastsupportingobject = player[cp].psupportingobject;

	// cheat code to always fall down
    if (getoption(OPT_CHEATS) && keytable[KPLUS])
		{ player[cp].pfalling = 1; player[cp].pjustfell = 1; }

	// ** if the player should be falling, well what are we waiting for?
    //    make him fall! **

    if (player[cp].pfalling)
    {  // nothing solid under player, let's make him fall
       player[cp].psemisliding = 0;
		
		// just now started falling? (wasn't falling last time)
		if (player[cp].plastfalling == 0)
		{
			// set initial fall speed and make the AAAAAUUUUHHHHH noise
			player[cp].pfallspeed = 1;
			player[cp].pfallspeed_increasetimer = 0;
			if (!player[cp].pjustjumped)
			{
				sound_play(SOUND_KEEN_FALL, PLAY_NOW);
			}
		}
		
		// gradully increase the fall speed up to maximum rate
		if (player[cp].pfallspeed_increasetimer>PFALL_INCREASERATE)
		{
			if (player[cp].pfallspeed<PFALL_MAXSPEED)
			{
				player[cp].pfallspeed++;
			}
			player[cp].pfallspeed_increasetimer=0;
		}
		else player[cp].pfallspeed_increasetimer++;
		
       // add current fall speed to player Y
       player[cp].y += player[cp].pfallspeed;
		
    }
    else
    {  // not falling
		
		if (player[cp].plastfalling)
		{  // just now stopped falling
			if (player[cp].pdie != PDIE_FELLOFFMAP)
				sound_stop(SOUND_KEEN_FALL);  // terminate fall noise
			// thud noise
			if (!player[cp].ppogostick) sound_play(SOUND_KEEN_LAND, PLAY_NOW);
			
			// fix "sliding" effect when you fall, go one way, then
			// before you land turn around and as you hit the ground
			// you're starting to move the other direction
			// (set inertia to 0 if it's contrary to player's current dir)
			if (player[cp].pinertia_x < 0 && player[cp].pshowdir==RIGHT)
				{ player[cp].pinertia_x = 0; }
			if (player[cp].pinertia_x > 0 && player[cp].pshowdir==LEFT)
				{ player[cp].pinertia_x = 0; }
		}
		
		// set psliding if we're on ice
		if (tiles[player[cp].psupportingtile].ice)
		{
			player[cp].psliding = 1;
			player[cp].pshowdir = player[cp].pdir;
		}
		else
		{
			player[cp].psliding = 0;
		}
	   
		// set psemisliding if we're on an snowy block
		if (tiles[player[cp].psupportingtile].semiice)
		{
			player[cp].psemisliding = 1;
		}
		else
		{
			player[cp].psemisliding = 0;
		}
		
	}   // close "not falling"

    // save fall state so we can detect the high/low-going edges
    player[cp].plastfalling = player[cp].pfalling;

    // ensure no sliding if we fall or jump off of ice
    if (player[cp].pfalling||player[cp].pjumping) player[cp].psliding=0;
}

// wouldn't it be cool if keen had a raygun, and he could shoot things?
// oh wait, he does, and here's the code for it.
void gamepdo_raygun(int cp)
{
int o;
int canRefire;

   if (player[cp].pfireframetimer) player[cp].pfireframetimer--;

   // FIRE button down, and not keencicled?
   if (player[cp].keytable[PKFIRE] && !player[cp].pfrozentime)
   {
     player[cp].inhibitwalking = 1;    // prevent moving
     player[cp].pfiring = 1;           // flag that we're firing
     player[cp].ppogostick = 0;        // put away pogo stick if out

     if (!player[cp].lastkeytable[PKFIRE] || getoption(OPT_FULLYAUTOMATIC))
     { // fire is newly pressed                   

       // limit how quickly shots can be fired
       if (getoption(OPT_FULLYAUTOMATIC))
       {
         if (player[cp].pfireframetimer < PFIRE_LIMIT_SHOT_FREQ_FA)
         {
           canRefire = 1;
         }
         else canRefire = 0;
       }
       else
       {
         if (player[cp].pfireframetimer < PFIRE_LIMIT_SHOT_FREQ)
         {
           canRefire = 1;
         }
         else canRefire = 0;
       }

       if (canRefire)
       {
          // show raygun for a minimum time even if FIRE is immediatly released
          player[cp].pfireframetimer = PFIRE_SHOWFRAME_TIME;

          // try to fire off a blast
          if (player[cp].inventory.charges)
          {  // we have enough charges

             player[cp].inventory.charges--;
             player[cp].pshowdir = player[cp].pdir;

              sound_play(SOUND_KEEN_FIRE, PLAY_NOW);
              if (player[cp].pdir==RIGHT)
              {  // fire a blast to the right
                 o = spawn_object(player[cp].x+((sprites[0].xsize-4)<<CSF), player[cp].y+(9<<CSF), OBJ_RAY);
                 objects[o].ai.ray.direction = RIGHT;
              }
              else
              {  // fire a blast to the left
                 o = spawn_object(player[cp].x-(12<<CSF), player[cp].y+(9<<CSF), OBJ_RAY);
                 objects[o].ai.ray.direction = LEFT;
              }
              // if '-nopk' argument set don't kill other players
              if (getoption(OPT_ALLOWPKING))
              {
                objects[o].ai.ray.dontHitEnable = 0;
              }
              else
              {
                objects[o].ai.ray.dontHitEnable = 1;
                objects[o].ai.ray.dontHit = OBJ_PLAYER;
              }
          }
          else
          { // oh shit, out of bullets
			// click!
			sound_play(SOUND_GUN_CLICK, PLAY_NOW);
          }  // end "do we have charges?"

       } // end "limit how quickly shots can be fired"

     } // end "fire is newly pressed"
   } // end "fire button down and not keencicled"
   else
   { // FIRE button is NOT down
      // put away ray gun after it's shown for the minimum period of time
      if (!player[cp].pfireframetimer)
      {  // ray gun shown for minimum time
        player[cp].pfiring = 0;
      }
      else
      {  // minimum time not expired
        player[cp].pfiring = 1;
        player[cp].inhibitwalking = 1;
      }
   }

}

// select the appropriate player frame based on what he's doing
void gamepdo_SelectFrame(int cp)
{
    player[cp].playframe = PSTANDFRAME;      // basic standing

    // select the frame assuming he's pointing right. ep1 does not select
    // a walk frame while fading--this is for the bonus teleporter in L13.
    if (player[cp].pdie)
	{
		player[cp].playframe = PDIEFRAME + player[cp].pdieframe;
	}
    else
    {
        if (player[cp].pfrozentime) player[cp].playframe = PFRAME_FROZEN + player[cp].pfrozenframe;
        else if (player[cp].pfiring) player[cp].playframe = PFIREFRAME;
        else if (player[cp].ppogostick) player[cp].playframe = PFRAME_POGO + (player[cp].pjumping==PPREPAREPOGO);
        else if (player[cp].pjumping) player[cp].playframe += player[cp].pjumpframe;
        else if (player[cp].pfalling) player[cp].playframe += 13;
        else if (player[cp].pwalking || player[cp].playpushed_x)
		{
			player[cp].playframe += player[cp].pwalkframe;
		}
    }

    // if he's going left switch the frame selected above to the
    // appropriate one for the left direction
    if (player[cp].pshowdir && !player[cp].pdie && !player[cp].pfrozentime)
    {
		if (player[cp].pfiring)
		{
			player[cp].playframe++;
		}
		else if (player[cp].ppogostick)
		{
			player[cp].playframe += 2;
		}
		else if (player[cp].pjumping || player[cp].pfalling)
		{
			player[cp].playframe += 6;
		}
		else
		{
			player[cp].playframe += 4;
		}
	}
}

// handles walking. the walking animation is handled by gamepdo_walkinganim()
void gamepdo_walking(int cp)
{
int cur_pfastincrate;
    if (player[cp].inhibitwalking && !player[cp].psliding)
    {
      if (!player[cp].pfrozentime||levelcontrol.episode!=1)
        if (!player[cp].pjumping && !player[cp].pfalling)
          player[cp].pinertia_x = 0;
      return;
    }

    // this prevents a "slipping" effect if you jump, say, right, then
    // start walking left just as you hit the ground
    if (player[cp].pjustjumped && ((player[cp].pinertia_x > 0 && player[cp].pdir==LEFT) ||\
                        (player[cp].pinertia_x < 0 && player[cp].pdir==RIGHT)))\
    {
      player[cp].pinertia_x = 0;
    }

    // this code makes it so that if you jump/fall onto a semi-sliding
    // block you'll start moving a little
    if (!player[cp].pjumping && !player[cp].pfalling)
    {
      // on left/right pressed, clear pjustjumped
      if (player[cp].keytable[PKLEFT] || player[cp].keytable[PKRIGHT])
      {
        player[cp].pjustjumped = 0;
        player[cp].pjustfell = 0;
      }

      // if we fall onto a semislide tile with no inertia
      // start moving a little
      if (player[cp].pjustfell && player[cp].psemisliding)
      {
        if (player[cp].pdir==RIGHT)
        {
          if (player[cp].blockedr)
          {
            player[cp].pjustjumped = 0;
            player[cp].pjustfell = 0;
          }
          else
          {
            if (!player[cp].pinertia_x) player[cp].pinertia_x = 1;
            player[cp].pshowdir = player[cp].pdir;
          }
        }
        else
        {
          if (player[cp].blockedl)
          {
            player[cp].pjustjumped = 0;
            player[cp].pjustfell = 0;
          }
          else
          {
            if (!player[cp].pinertia_x) player[cp].pinertia_x = -1;
            player[cp].pshowdir = player[cp].pdir;
          }
        }
      }
    }

    // test if we're trying to walk
    if ((player[cp].psemisliding&&player[cp].pinertia_x!=0) || \
		((player[cp].keytable[PKLEFT] || player[cp].keytable[PKRIGHT] || \
		((player[cp].keytable[PKUP] || player[cp].keytable[PKDOWN]) && map.isworldmap)) \
		&& !player[cp].inhibitwalking))
    {
      // we just started walking or we changed directions suddenly?
      if (player[cp].pwalking == 0 || ((player[cp].lastpdir==RIGHT && player[cp].pdir==LEFT)||(player[cp].lastpdir==LEFT && player[cp].pdir==RIGHT)))
      {
        player[cp].pwalkanimtimer = 0;
        player[cp].pwalkframe = 1;
        player[cp].pwalkincreasetimer = 0;
        player[cp].pfriction_timer_x = 0;
        player[cp].pfriction_timer_y = 0;
        if (!player[cp].pjumping && !player[cp].pfalling)
        {
          player[cp].pinertia_x = 0;
          player[cp].pinertia_y = 0;
        }
      }
      player[cp].lastpdir = player[cp].pdir;
      player[cp].pwalking = 1;
    }
    else
    {   // end "d-pad down and not sliding"
      player[cp].pwalking = 0;
    }

      /* when sliding on ice force maximum speed */
      if (player[cp].psliding)
      {
         if (player[cp].pjumping != PPREPAREJUMP &&\
             player[cp].pjumping != PPREPAREPOGO)
         {
           // reset walk frame because we have no walk animation while on ice
           player[cp].pwalkframe = 0;
           // keep player sliding at maximum speed
           if (player[cp].pdir==RIGHT)
           {
             player[cp].pinertia_x = PMAXSPEED;
           }
           else if (player[cp].pdir==LEFT)
           {
             player[cp].pinertia_x = -PMAXSPEED;
           }
         }
         return;
      }
      else if (!player[cp].pwalking) return;    // don't run rest of sub if not walking
      // if we get here we're walking and not sliding

      /* increase player inertia while walk key held down */
      if (player[cp].ppogostick)
        cur_pfastincrate = PFASTINCRATE_POGO;
      else
        cur_pfastincrate = PFASTINCRATE;

      if (player[cp].keytable[PKRIGHT])
      { // RIGHT key down

          // prevent sliding on map
          if (map.isworldmap && player[cp].pinertia_x < 0) player[cp].pinertia_x = 0;

          // quickly reach PFASTINCMAXSPEED
          if (player[cp].pwalkincreasetimer>=cur_pfastincrate && player[cp].pinertia_x<PFASTINCMAXSPEED)
          {
             player[cp].pinertia_x++;
             player[cp].pwalkincreasetimer=0;
          }
          else
          {
             player[cp].pwalkincreasetimer++;
          }
          // increase up to max speed every time frame is changed
          if (!player[cp].pwalkanimtimer && player[cp].pinertia_x < PMAXSPEED)
          {
             player[cp].pinertia_x++;
          }
      }
      else if (player[cp].keytable[PKLEFT])
      {
          // prevent sliding on map
          if (map.isworldmap && player[cp].pinertia_x > 0) player[cp].pinertia_x = 0;

          // quickly reach PFASTINCMAXSPEED
          if (player[cp].pwalkincreasetimer>=cur_pfastincrate && player[cp].pinertia_x>-PFASTINCMAXSPEED)
          {
             player[cp].pinertia_x--;
             player[cp].pwalkincreasetimer=0;
          }
          else
          {
             player[cp].pwalkincreasetimer++;
          }
          // increase up to max speed every time frame is changed
          if (!player[cp].pwalkanimtimer && player[cp].pinertia_x>-PMAXSPEED)
          {
             player[cp].pinertia_x--;
          }
      }

      if (player[cp].keytable[PKDOWN])
      {
          if (map.isworldmap && player[cp].pinertia_y < 0) player[cp].pinertia_y = 0;
          // quickly reach PFASTINCMAXSPEED
          if (player[cp].pwalkincreasetimer>=PFASTINCRATE && player[cp].pinertia_y<PFASTINCMAXSPEED)
          {
             player[cp].pinertia_y++;
             player[cp].pwalkincreasetimer=0;
          }
          else
          {
             player[cp].pwalkincreasetimer++;
          }
          // increase up to max speed every time frame is changed
          if (!player[cp].pwalkanimtimer && player[cp].pinertia_y<PMAXSPEED)
          {
             player[cp].pinertia_y++;
          }
      }
      else if (player[cp].keytable[PKUP])
      {
          if (map.isworldmap && player[cp].pinertia_y > 0) player[cp].pinertia_y = 0;
          // quickly reach PFASTINCMAXSPEED
          if (player[cp].pwalkincreasetimer>=PFASTINCRATE && player[cp].pinertia_y>-PFASTINCMAXSPEED)
          {
             player[cp].pinertia_y--;
             player[cp].pwalkincreasetimer=0;
          }
          else
          {
             player[cp].pwalkincreasetimer++;
          }
          // increase up to max speed every time frame is changed
          if (!player[cp].pwalkanimtimer && player[cp].pinertia_y>-PMAXSPEED)
          {
             player[cp].pinertia_y--;
          }
      }

}

void gamepdo_ankh(int cp)
{
int o;
	if (!player[cp].ankhtime) return;

	o = player[cp].ankhshieldobject;
	if (levelcontrol.episode==3)
	{
		objects[o].x = player[cp].x - (8<<CSF);
		objects[o].y = player[cp].y - (8<<CSF);
	}
	else
	{
		objects[o].x = player[cp].x - (2<<CSF);
		objects[o].y = player[cp].y + (2<<CSF);
	}

	player[cp].ankhtime--;
	if (!player[cp].ankhtime)
	{
		delete_object(o);
	}
	else if (player[cp].ankhtime < ANKH_STAGE3_TIME)
	{
		objects[o].ai.se.state = ANKH_STATE_FLICKERSLOW;
	}
	else if (player[cp].ankhtime < ANKH_STAGE2_TIME)
	{
		objects[o].ai.se.state = ANKH_STATE_FLICKERFAST;
	}
	else
	{
		objects[o].ai.se.state = ANKH_STATE_NOFLICKER;
	}
}
