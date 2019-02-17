#include "../keen.h"
#include "butler.fdh"

// AI for "butler" robot (ep1)

#define BUTLER_TURN     0
#define BUTLER_WALK     1

#define BUTLER_WALK_SPEED        7
#define BUTLER_WALK_SPEED_FAST   11
#define BUTLER_WALK_ANIM_TIME    35
#define BUTLER_WALK_ANIM_TIME_FAST    25
#define BUTLER_TURN_TIME         50

#define BUTLERPUSHAMOUNT         22
#define BUTLERPUSHAMOUNTFAST     15

// distance in pixels butler should look ahead to avoid falling
// off an edge
#define BUTLER_LOOK_AHEAD_DIST   1

// frames
#define BUTLER_WALK_LEFT_FRAME   92
#define BUTLER_WALK_RIGHT_FRAME  88
#define BUTLER_TURNLEFT_FRAME    96
#define BUTLER_TURNRIGHT_FRAME   97

void butler_ai(int o)
{
char not_about_to_fall;
   if (objects[o].needinit)
   {
     objects[o].ai.butler.state = BUTLER_WALK;
     objects[o].ai.butler.movedir = RIGHT;
     objects[o].ai.butler.animtimer = 0;
     objects[o].inhibitfall = 1;
     objects[o].canbezapped = 1;  // will stop bullets but are not harmed
     objects[o].needinit = 0;
   }
   // push keen
     if (objects[o].touchPlayer && !player[objects[o].touchedBy].pdie)
     {
          sound_play(SOUND_YORP_BUMP, PLAY_NORESTART);

          if (player[primaryplayer].x < objects[o].x)
          {
            player[objects[o].touchedBy].playpushed_x = -BUTLERPUSHAMOUNT;
            if (getoption(OPT_HARD)) player[objects[o].touchedBy].playpushed_x -= BUTLERPUSHAMOUNTFAST;
            player[objects[o].touchedBy].playpushed_decreasetimer = 0;
            player[objects[o].touchedBy].pdir = player[objects[o].touchedBy].pshowdir = LEFT;
          }
          else
          {
            player[objects[o].touchedBy].playpushed_x = BUTLERPUSHAMOUNT;
            if (getoption(OPT_HARD)) player[objects[o].touchedBy].playpushed_x += BUTLERPUSHAMOUNTFAST;
            player[objects[o].touchedBy].playpushed_decreasetimer = 0;
            player[objects[o].touchedBy].pdir = player[objects[o].touchedBy].pshowdir = RIGHT;
          }
     }

   switch(objects[o].ai.butler.state)
   {
    case BUTLER_TURN:
       if (objects[o].ai.butler.timer > BUTLER_TURN_TIME)
       {
         objects[o].ai.butler.movedir ^= 1;
         objects[o].ai.butler.animtimer = 0;
         objects[o].ai.butler.state = BUTLER_WALK;
       } else objects[o].ai.butler.timer++;
    break;
    case BUTLER_WALK:
       if (objects[o].ai.butler.movedir==LEFT)
       {  // move left
         not_about_to_fall = tiles[getmaptileat((objects[o].x>>CSF)-BUTLER_LOOK_AHEAD_DIST, (objects[o].y>>CSF)+sprites[BUTLER_WALK_LEFT_FRAME].ysize)].solidfall;
         objects[o].sprite = BUTLER_WALK_LEFT_FRAME + objects[o].ai.butler.frame;
         if (!objects[o].blockedl && not_about_to_fall)
         {
            if (getoption(OPT_HARD))
              objects[o].x -= BUTLER_WALK_SPEED_FAST;
            else
              objects[o].x -= BUTLER_WALK_SPEED;
         }
         else
         {
           objects[o].sprite = BUTLER_TURNRIGHT_FRAME;
           objects[o].ai.butler.frame = 0;
           objects[o].ai.butler.timer = 0;
           objects[o].ai.butler.animtimer = 0;
           objects[o].ai.butler.state = BUTLER_TURN;
         }
       }
       else
       {  // move right
         not_about_to_fall = tiles[getmaptileat((objects[o].x>>CSF)+sprites[BUTLER_WALK_RIGHT_FRAME].xsize+BUTLER_LOOK_AHEAD_DIST, (objects[o].y>>CSF)+sprites[BUTLER_WALK_RIGHT_FRAME].ysize)].solidfall;
         objects[o].sprite = BUTLER_WALK_RIGHT_FRAME + objects[o].ai.butler.frame;
         if (!objects[o].blockedr && not_about_to_fall)
         {
            if (getoption(OPT_HARD))
              objects[o].x += BUTLER_WALK_SPEED_FAST;
            else
              objects[o].x += BUTLER_WALK_SPEED;
         }
         else
         {
           objects[o].sprite = BUTLER_TURNLEFT_FRAME;
           objects[o].ai.butler.frame = 0;
           objects[o].ai.butler.timer = 0;
           objects[o].ai.butler.animtimer = 0;
           objects[o].ai.butler.state = BUTLER_TURN;
         }
       }
       // walk animation
       if (objects[o].ai.butler.animtimer > BUTLER_WALK_ANIM_TIME || \
           (objects[o].ai.butler.animtimer > BUTLER_WALK_ANIM_TIME_FAST && getoption(OPT_HARD)))
       {
         if (objects[o].ai.butler.frame>=3) objects[o].ai.butler.frame=0;
                                     else objects[o].ai.butler.frame++;
         objects[o].ai.butler.animtimer = 0;
       } else objects[o].ai.butler.animtimer++;
    break;
   }
}

