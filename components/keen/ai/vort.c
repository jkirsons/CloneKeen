#include "../keen.h"
#include "vort.h"
#include "vort.fdh"
#include "vort.fdh"

// Vorticon (all episodes, albeit the behavior changes slightly
// depending on levelcontrol.episode).

void vort_ai(int o)
{
int bonk,kill;
	if (objects[o].needinit)
	{  // first time initilization
		objects[o].ai.vort.frame = 0;
		objects[o].ai.vort.animtimer = 0;
		objects[o].ai.vort.state = VORT_LOOK;
		objects[o].ai.vort.timer = 0;
		objects[o].ai.vort.dist_traveled = VORT_TRAPPED_DIST+1;
		objects[o].canbezapped = 1;
		objects[o].needinit = 0;

		// copy in animation frame indexes for the current ep
		if (levelcontrol.episode==1)
		{
			objects[o].ai.vort.WalkLeftFrame = VORT1_WALK_LEFT_FRAME;
			objects[o].ai.vort.WalkRightFrame = VORT1_WALK_RIGHT_FRAME;
			objects[o].ai.vort.LookFrame = VORT1_LOOK_FRAME;
			objects[o].ai.vort.JumpRightFrame = VORT1_JUMP_RIGHT_FRAME;
			objects[o].ai.vort.JumpLeftFrame = VORT1_JUMP_LEFT_FRAME;
			objects[o].ai.vort.DyingFrame = VORT1_DYING_FRAME;
		}
		else if (levelcontrol.episode==2)
		{
			objects[o].ai.vort.WalkLeftFrame = VORT2_WALK_LEFT_FRAME;
			objects[o].ai.vort.WalkRightFrame = VORT2_WALK_RIGHT_FRAME;
			objects[o].ai.vort.LookFrame = VORT2_LOOK_FRAME;
			objects[o].ai.vort.JumpRightFrame = VORT2_JUMP_RIGHT_FRAME;
			objects[o].ai.vort.JumpLeftFrame = VORT2_JUMP_LEFT_FRAME;
			objects[o].ai.vort.DyingFrame = VORT2_DYING_FRAME;
			objects[o].ai.vort.DeadFrame = VORT2_DEAD_FRAME;
		}
		else if (levelcontrol.episode==3)
		{
			objects[o].ai.vort.WalkLeftFrame = VORT3_WALK_LEFT_FRAME;
			objects[o].ai.vort.WalkRightFrame = VORT3_WALK_RIGHT_FRAME;
			objects[o].ai.vort.LookFrame = VORT3_LOOK_FRAME;
			objects[o].ai.vort.JumpRightFrame = VORT3_JUMP_RIGHT_FRAME;
			objects[o].ai.vort.JumpLeftFrame = VORT3_JUMP_LEFT_FRAME;
			objects[o].ai.vort.DyingFrame = VORT3_DYING_FRAME;
			objects[o].ai.vort.DeadFrame = VORT3_DEAD_FRAME;
		}
	}
	
	if (objects[o].ai.vort.state==VORT_DEAD) return;

   if (objects[o].canbezapped)
   {
     kill = 0;
	 if (levelcontrol.episode==1 && levelcontrol.curlevel==FINAL_LEVEL)
	 {
		if (objects[o].zapped >= VORT_COMMANDER_HP) kill = 1;
	 }
	 else if (levelcontrol.episode==1)
	 {
		if (objects[o].zapped >= VORT_HP) kill = 1;
	 }
	 else if (getoption(OPT_HARD))
	 {
		if (objects[o].zapped >= VORT_HARD_HP) kill = 1;
	 }
	 else if (objects[o].zapped)
	 {
		kill = 1;
	 }
     else if (levelcontrol.episode==2 && getmaptileat((objects[o].x>>CSF)+12, (objects[o].y>>CSF)+16)==TILE_GLOWCELL)
     {
		kill = 1;
     }

     if (kill)
     {
       objects[o].inhibitfall = 0;
       objects[o].canbezapped = 0;
       objects[o].ai.vort.animtimer = 0;
       objects[o].ai.vort.frame = 0;
       if (levelcontrol.episode == 1)
       {
			objects[o].ai.vort.state = VORT_DYING;
			if (levelcontrol.cepvars.VortDieDoFade) fade(FADE_FLASH, FADE_SLOW);
       }
       else
       {
			objects[o].ai.vort.state = VORT2_DYING;
			if (getoption(OPT_HARD)) fade(FADE_FLASH, FADE_NORM);
	   }
       sound_play(SOUND_VORT_DIE, PLAY_NOW);
     }
   }
   
   // deadly to the touch
   if (objects[o].touchPlayer && objects[o].canbezapped)
   {
		killplayer(objects[o].touchedBy);
   }

vort_reprocess: ;
   switch(objects[o].ai.vort.state)
   {
    case VORT_JUMP:
       if (objects[o].ai.vort.dir == RIGHT)
       {
         if (!objects[o].blockedr) objects[o].x += VORT_WALK_SPEED;
       }
       else
       {
         if (!objects[o].blockedl) objects[o].x -= VORT_WALK_SPEED;
       }

       if (objects[o].ai.vort.inertiay>0 && objects[o].blockedd)
       {  // The Vorticon Has Landed!
          objects[o].inhibitfall = 0;
          objects[o].ai.vort.state = VORT_LOOK;
          goto vort_reprocess;
       }
       // check if the vorticon has bonked into a ceiling, if so,
       // immediately terminate the jump
       bonk = 0;
       if (tiles[getmaptileat((objects[o].x>>CSF)+1, (objects[o].y>>CSF))].solidceil) bonk = 1;
       else if (tiles[getmaptileat((objects[o].x>>CSF)+16, (objects[o].y>>CSF))].solidceil) bonk = 1;
       else if (tiles[getmaptileat((objects[o].x>>CSF)+23, (objects[o].y>>CSF))].solidceil) bonk = 1;
       if (bonk && objects[o].ai.vort.inertiay < 0)
       {
          objects[o].ai.vort.inertiay = 0;
       }

       // apply Y inertia
       objects[o].y += objects[o].ai.vort.inertiay;
          
       if (objects[o].ai.vort.timer > VORT_JUMP_FRICTION)
       { // slowly decrease upgoing rate
          if (objects[o].ai.vort.inertiay < VORT_MAX_FALL_SPEED)
          {
			objects[o].ai.vort.inertiay++;
          }
          objects[o].ai.vort.timer = 0;
       }
       else objects[o].ai.vort.timer++;
    break;
    case VORT_LOOK:
       objects[o].sprite = objects[o].ai.vort.LookFrame + objects[o].ai.vort.frame;

       if (objects[o].ai.vort.animtimer > VORT_LOOK_ANIM_TIME)
       {
         if (objects[o].ai.vort.frame > 0)
         {
           if (objects[o].blockedl)
             { objects[o].ai.vort.dir = RIGHT; }
           else if (objects[o].blockedr)
             { objects[o].ai.vort.dir = LEFT; }
           else
           { // not blocked on either side, head towards player
             if (player[primaryplayer].x < objects[o].x)
               { objects[o].ai.vort.dir = LEFT; }
             else
               { objects[o].ai.vort.dir = RIGHT; }
           }
           objects[o].ai.vort.timer = 0;
           objects[o].ai.vort.frame = 0;
           objects[o].ai.vort.state = VORT_WALK;
         } else objects[o].ai.vort.frame++;
         objects[o].ai.vort.animtimer = 0;
       } else objects[o].ai.vort.animtimer++;
       break;
     case VORT_WALK:
       objects[o].ai.vort.dist_traveled++;

       if (rnd()%VORT_JUMP_PROB == (VORT_JUMP_PROB/2))
       {  // let's jump.
         if (!levelcontrol.dark && !objects[o].blockedu)
         {
           vort_initiatejump(o);
           goto vort_reprocess;
         }
       }
       if (objects[o].ai.vort.dir==LEFT)
       {  // move left
         objects[o].sprite = objects[o].ai.vort.WalkLeftFrame + objects[o].ai.vort.frame;

         if (!objects[o].blockedl)
         {
           objects[o].x -= VORT_WALK_SPEED;
         }
         else
         {
           objects[o].ai.vort.frame = 0;
           objects[o].ai.vort.animtimer = 0;
           objects[o].ai.vort.state = VORT_LOOK;

           // if we only traveled a tiny amount before hitting a wall, we've
           // probably fallen into a small narrow area, and we need to try
           // to jump out of it
           if (objects[o].ai.vort.dist_traveled < VORT_TRAPPED_DIST && !levelcontrol.dark && objects[o].blockedd && !objects[o].blockedu)
           {
             vort_initiatejump(o);
             if (rnd()&1)
             {
               objects[o].ai.vort.inertiay = -VORT_MAX_JUMP_HEIGHT;
             }
             else
             {
               objects[o].ai.vort.inertiay = -VORT_MIN_JUMP_HEIGHT;
             }
             goto vort_reprocess;
           }            
           else objects[o].ai.vort.dist_traveled = 0;
         }
       }
       else
       {  // move right
         objects[o].sprite = objects[o].ai.vort.WalkRightFrame + objects[o].ai.vort.frame;

         if (!objects[o].blockedr)
         {
           objects[o].x += VORT_WALK_SPEED;
         }
         else
         {
           objects[o].ai.vort.frame = 0;
           objects[o].ai.vort.animtimer = 0;
           objects[o].ai.vort.state = VORT_LOOK;

           if (objects[o].ai.vort.dist_traveled < VORT_TRAPPED_DIST && !levelcontrol.dark && objects[o].blockedd && !objects[o].blockedu)
           {
             vort_initiatejump(o);
             if (rnd()&1)
             {
               objects[o].ai.vort.inertiay = -VORT_MAX_JUMP_HEIGHT;
             }
             else
             {
               objects[o].ai.vort.inertiay = -VORT_MIN_JUMP_HEIGHT;
             }
             goto vort_reprocess;
           }            
           else objects[o].ai.vort.dist_traveled = 0;
         }
       }
       // walk animation
       if (objects[o].ai.vort.animtimer > VORT_WALK_ANIM_TIME)
       {
         if (objects[o].ai.vort.frame>=3) objects[o].ai.vort.frame=0;
                                     else objects[o].ai.vort.frame++;
         objects[o].ai.vort.animtimer = 0;
       } else objects[o].ai.vort.animtimer++;
    break;
    case VORT_DYING:
    objects[o].sprite = objects[o].ai.vort.DyingFrame + objects[o].ai.vort.frame;
       
       if (objects[o].ai.vort.animtimer > VORT_DIE_ANIM_TIME)
       { 
         objects[o].ai.vort.frame++;
         if (objects[o].ai.vort.frame>=6)
         {
             objects[o].ai.vort.state = VORT_DEAD;
         }
         objects[o].ai.vort.animtimer = 0;
       } else objects[o].ai.vort.animtimer++;
    break;
    case VORT2_DYING:
       objects[o].sprite = objects[o].ai.vort.DyingFrame;
       if (objects[o].ai.vort.animtimer > VORT2_DIE_ANIM_TIME)
       {
         objects[o].sprite = objects[o].ai.vort.DeadFrame;
         objects[o].ai.vort.state = VORT_DEAD; 
       }
       else
       {
         objects[o].ai.vort.animtimer++;
       }
    break;
    default: break;
   }
}

void vort_initiatejump(int o)
{
	// must be standing on floor to jump
	if (!objects[o].blockedd) return;

	objects[o].ai.vort.frame = 0;
	objects[o].ai.vort.animtimer = 0;
	objects[o].ai.vort.inertiay = \
	 -((rnd()%(VORT_MAX_JUMP_HEIGHT-VORT_MIN_JUMP_HEIGHT))+VORT_MIN_JUMP_HEIGHT);

	if (objects[o].ai.vort.dir==RIGHT)
		objects[o].sprite = objects[o].ai.vort.JumpRightFrame;
	else
		objects[o].sprite = objects[o].ai.vort.JumpLeftFrame;

	objects[o].inhibitfall = 1;
	objects[o].ai.vort.state = VORT_JUMP;
}

