#include "../keen.h"
#include "foob.fdh"

// AI for the foobs (yellow "scaredy cat" creatures) (ep3)
#define FOOB_WALK    0
#define FOOB_SPOOK   1
#define FOOB_FLEE    2
#define FOOB_EXPLODE 3
#define FOOB_DEAD    4

#define FOOB_WALK_SPEED      2
#define FOOB_WALK_ANIM_RATE  80

#define FOOB_FLEE_SPEED      18
#define FOOB_FLEE_ANIM_RATE  16

#define FOOB_SPOOK_SHOW_TIME    50

#define FOOB_HARDMODE_BLOCK_TIME	150

#define FOOB_EXPLODE_ANIM_RATE  35

#define FOOB_SPOOK_TIME         150
#define FOOB_RELAX_TIME         800

#define FOOB_WALK_LEFT_FRAME    93
#define FOOB_WALK_RIGHT_FRAME   95
#define FOOB_SPOOK_FRAME        97
#define FOOB_EXPLODE_FRAME      97
#define FOOB_DEAD_FRAME         101

void foob_ai(int o)
{
int i, onsamelevel;
  if (objects[o].needinit)
  {
    objects[o].ai.foob.state = FOOB_WALK;
    objects[o].ai.foob.dir = RIGHT;
    objects[o].ai.foob.animframe = 0;
    objects[o].ai.foob.animtimer = 0;
    objects[o].ai.foob.OnSameLevelTime = 0;
    objects[o].blockedr = 0;
    objects[o].canbezapped = 1;
	objects[o].dead = 0;
    objects[o].needinit = 0;
  }
  if (objects[o].ai.foob.state==FOOB_DEAD) return;
	if (objects[o].ai.foob.state != FOOB_EXPLODE) levelcontrol.numyorps++;
	if (!objects[o].hasbeenonscreen) return;

  if (objects[o].zapped || objects[o].touchPlayer)
  {
    if (objects[o].ai.foob.state != FOOB_EXPLODE)
    {
      objects[o].ai.foob.animframe = 0;
      objects[o].ai.foob.animtimer = 0;
      objects[o].ai.foob.state = FOOB_EXPLODE;
      objects[o].canbezapped = 0;
      if (objects[o].onscreen) sound_play(SOUND_YORP_DIE, PLAY_NOW);
      if (getoption(OPT_HARD) && objects[o].touchPlayer)
      {
        killplayer(objects[o].touchedBy);
      }
    }
  }

   // find out if a player is on the same level as the foob cat
   onsamelevel = 0;
   for(i=0;i<numplayers;i++)
   {
     if (player[i].isPlaying)
     {
       if ((player[i].y >= objects[o].y-(24<<CSF)) && ((player[i].y>>CSF)+sprites[0].ysize <= (objects[o].y>>CSF)+sprites[objects[o].sprite].ysize+24))
       {
         onsamelevel = 1;
         objects[o].ai.foob.SpookedByWho = i;
         break;
       }
     }
   }

  switch(objects[o].ai.foob.state)
  {
   case FOOB_WALK:
     // if player is on the same level for FOOB_SPOOK_TIME, run away
     if (onsamelevel)
     {
       if (objects[o].ai.foob.OnSameLevelTime > FOOB_SPOOK_TIME)
       {
         objects[o].ai.foob.state = FOOB_SPOOK;
         objects[o].ai.foob.spooktimer = 0;
         if (objects[o].onscreen) sound_play(SOUND_YORP_DIE, PLAY_NOW);
       }
       else objects[o].ai.foob.OnSameLevelTime++;
     }
     else objects[o].ai.foob.OnSameLevelTime = 0;

     if (objects[o].ai.foob.dir == RIGHT)
     {  // walking right
       objects[o].sprite = FOOB_WALK_RIGHT_FRAME + objects[o].ai.foob.animframe;
       if (objects[o].blockedr)
       {
         objects[o].ai.foob.dir = LEFT;
       }
       else
       {
         objects[o].x += FOOB_WALK_SPEED;
       }
     }
     else
     {  // walking left
       objects[o].sprite = FOOB_WALK_LEFT_FRAME + objects[o].ai.foob.animframe;
       if (objects[o].blockedl)
       {
         objects[o].ai.foob.dir = RIGHT;
       }
       else
       {
         objects[o].x -= FOOB_WALK_SPEED;
       }
     }

     /* walk animation */
     if (objects[o].ai.foob.animtimer > FOOB_WALK_ANIM_RATE)
     {
       objects[o].ai.foob.animframe ^= 1;
       objects[o].ai.foob.animtimer = 0;
     }
     else objects[o].ai.foob.animtimer++;
   break;

   case FOOB_SPOOK:
      objects[o].sprite = FOOB_SPOOK_FRAME;

      if (objects[o].ai.foob.spooktimer > FOOB_SPOOK_SHOW_TIME)
      {
         objects[o].ai.foob.state = FOOB_FLEE;
         objects[o].ai.foob.OffOfSameLevelTime = 0;
         // run away from the offending player
         if (player[objects[o].ai.foob.SpookedByWho].x < objects[o].x)
         {
           objects[o].ai.foob.dir = RIGHT;
         }
         else
         {
           objects[o].ai.foob.dir = LEFT;
         }
		 // in hard mode run TOWARDS the player (he's deadly in hard mode)
		 if (getoption(OPT_HARD)) objects[o].ai.foob.dir ^= 1;
      }
      else objects[o].ai.foob.spooktimer++;
   break;

   case FOOB_FLEE:
     // if player is off of the same level for FOOB_RELAX_TIME,
     // we can stop fleeing
     if (!onsamelevel)
     {
       if (objects[o].ai.foob.OffOfSameLevelTime > FOOB_RELAX_TIME)
       {
relax: ;
         objects[o].ai.foob.state = FOOB_WALK;
         objects[o].ai.foob.OnSameLevelTime = 0;
		 break;
       }
       else objects[o].ai.foob.OffOfSameLevelTime++;
     }
     else objects[o].ai.foob.OffOfSameLevelTime = 0;

     if (objects[o].ai.foob.dir == RIGHT)
     {  // walking right
       objects[o].sprite = FOOB_WALK_RIGHT_FRAME + objects[o].ai.foob.animframe;
       if (!objects[o].blockedr)
       {
			objects[o].x += FOOB_FLEE_SPEED;
			objects[o].ai.foob.blockedtime = 0;
       }
	   else if (getoption(OPT_HARD))
	   {
			if (++objects[o].ai.foob.blockedtime >= FOOB_HARDMODE_BLOCK_TIME)
			{
				objects[o].ai.foob.blockedtime = 0;
				goto relax;
			}
	   }
     }
     else
     {  // walking left
       objects[o].sprite = FOOB_WALK_LEFT_FRAME + objects[o].ai.foob.animframe;
       if (!objects[o].blockedl)
       {
			objects[o].x -= FOOB_FLEE_SPEED;
			objects[o].ai.foob.blockedtime = 0;
       }
	   else if (getoption(OPT_HARD))
	   {
			if (++objects[o].ai.foob.blockedtime >= FOOB_HARDMODE_BLOCK_TIME)
			{
				objects[o].ai.foob.blockedtime = 0;
				goto relax;
			}
	   }
     }

     /* walk animation */
     if (objects[o].ai.foob.animtimer > FOOB_FLEE_ANIM_RATE)
     {
       objects[o].ai.foob.animframe ^= 1;
       objects[o].ai.foob.animtimer = 0;
     }
     else objects[o].ai.foob.animtimer++;
   break;

   case FOOB_EXPLODE:
     // ahhhhh; I'm sorry.....you poor little thing......
     objects[o].sprite = FOOB_EXPLODE_FRAME + objects[o].ai.foob.animframe;
     if (objects[o].sprite==FOOB_DEAD_FRAME)
     {
        objects[o].ai.foob.state = FOOB_DEAD;
		objects[o].dead = 1;
		if (getoption(OPT_MUSTKILLYORPS))
		{
		   risebonus(PT100_SPRITE, objects[o].x-(2<<CSF), objects[o].y);
		   // fixme: points should be given to player who shot the yorp
		   // temphack
		   incscore(0, 100);
		}
     }
     else
     {
        if (objects[o].ai.foob.animtimer > FOOB_EXPLODE_ANIM_RATE)
        {
           objects[o].ai.foob.animframe++;
           objects[o].ai.foob.animtimer = 0;
        }
        else objects[o].ai.foob.animtimer++;
     }
   break;
  }
}

