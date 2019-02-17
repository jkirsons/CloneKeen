/* ESEQ_EP2.C
  Ending sequence for Episode 2.
*/

#include "keen.h"
#include "eseq_ep2.fdh"

#define CMD_MOVE                0
#define CMD_WAIT                1
#define CMD_SPAWNSPR            2
#define CMD_REMOVESPR           3
#define CMD_FADEOUT             4
#define CMD_ENDOFQUEUE          5
#define CMD_ENABLESCROLLING     6
#define CMD_DISABLESCROLLING    7
typedef struct stShipQueue
{
  int cmd;
  int time;
  int flag1;
} stShipQueue;
stShipQueue shipqueue[32];
int ShipQueuePtr;

#define LETTER_SHOW_SPD          30

// start x,y map scroll position for eseq2_TantalusRay()
#define TANTALUS_X       0
#define TANTALUS_Y       0

#define TANTALUS_SPD_X   21
#define TANTALUS_SPD_Y   9

#define TANTALUS_SPRITE  58

#define TAN_DELAY_BEFORE_FIRE   500

#define TAN_STATE_WAITBEFOREFIRE        0
#define TAN_STATE_FIRING                1
#define TAN_STATE_EARTH_EXPLODING       2
#define TAN_STATE_CHUNKS_FLYING         3
#define TAN_STATE_GAMEOVER              4

#define EARTHCHUNK_BIG_UP       64
#define EARTHCHUNK_BIG_DN       66
#define EARTHCHUNK_SMALL_UP     68
#define EARTHCHUNK_SMALL_DN     70

int eseq2_TantalusRay(void)
{
char enter,lastenterstate;
int x, y, t, o, i;
int tantalus_animframe, tantalus_animtimer;
int state, timer, spawnedcount;

  fadecontrol.mode = FADE_GO;
  fadecontrol.rate = FADE_NORM;
  fadecontrol.dir = FADE_OUT;
  fadecontrol.curamt = PAL_FADE_SHADES;
  fadecontrol.fadetimer = 0;
  do
  {
    gamedo_fades();
  } while(fadecontrol.mode!=FADE_COMPLETE);

  levelcontrol.dark = 0;
  pal_init(levelcontrol.dark);

  initgame();
  state = TAN_STATE_WAITBEFOREFIRE;

  showmapatpos(81, TANTALUS_X, TANTALUS_Y, 0);

  AllPlayersInvisible();  
  numplayers = 1;
  player[0].hideplayer = 0;
  objects[player[0].useObject].onscreen = 1;

  // place the player (which is actually the tantalus ray) at the mouth
  // of the vorticon mothership
  if (map_findtile(593, &x, &y))
  { // found the tile
    player[0].x = ((x<<4)-1)<<CSF; 
    player[0].y = ((y<<4)-0)<<CSF; 
  }
  else
  {
      crash("eseq2_TantalusRay(): unable to find tile 593.");
      return;
  }

  player[0].playframe = BLANKSPRITE;

  fadecontrol.mode = FADE_GO;
  fadecontrol.rate = FADE_NORM;
  fadecontrol.dir = FADE_IN;
  fadecontrol.curamt = 0;
  fadecontrol.fadetimer = 0;
  tantalus_animframe = 0;
  timer = 0;
  do
  {
    switch(state)
    {
    case TAN_STATE_WAITBEFOREFIRE:
       if (timer > TAN_DELAY_BEFORE_FIRE)
       {
          sound_play(SOUND_KEEN_FIRE, PLAY_NOW);
          state = TAN_STATE_FIRING;
          timer = 0;
       }
       else timer++;
    break;
    case TAN_STATE_FIRING:

       if (tantalus_animtimer>5)
       {
         tantalus_animframe ^= 1;
         player[0].playframe = TANTALUS_SPRITE + tantalus_animframe;
       }
       else tantalus_animtimer++;

       player[0].x += TANTALUS_SPD_X;
       player[0].y += TANTALUS_SPD_Y;

       t = getmaptileat((player[0].x>>CSF)+(sprites[TANTALUS_SPRITE].xsize/2), (player[0].y>>CSF)+(sprites[TANTALUS_SPRITE].ysize/2));
       if (t==586)
       {  // hit center of earth
         state = TAN_STATE_EARTH_EXPLODING;
         player[0].playframe = BLANKSPRITE;
         timer = 0;
         spawnedcount = 0;
         srnd(300);
         o = spawn_object(player[0].x+(24<<CSF), player[0].y-(8<<CSF), OBJ_EXPLOSION);
         sound_play(SOUND_EARTHPOW, PLAY_NOW);
       }
    break;
    case TAN_STATE_EARTH_EXPLODING:
      if (!timer)
      {
         if (spawnedcount<16) o = spawn_object(player[0].x+((rnd()%32)<<CSF), player[0].y+((rnd()%32)<<CSF)-(8<<CSF), OBJ_EXPLOSION);
         switch(spawnedcount)
         {
          case 0: o = spawn_object(player[0].x-(8<<CSF), player[0].y-(8<<CSF), OBJ_EXPLOSION); break;
          case 1: o = spawn_object(player[0].x+(24<<CSF), player[0].y+(4<<CSF), OBJ_EXPLOSION); break;
          case 2: o = spawn_object(player[0].x+(16<<CSF), player[0].y-(8<<CSF), OBJ_EXPLOSION); break;
          case 3: o = spawn_object(player[0].x+(24<<CSF), player[0].y+(16<<CSF), OBJ_EXPLOSION); break;
          case 4: o = spawn_object(player[0].x-(8<<CSF), player[0].y+(4<<CSF), OBJ_EXPLOSION); break;
          case 5:
           o = spawn_object(player[0].x-(8<<CSF), player[0].y+(16<<CSF), OBJ_EXPLOSION);
          // spawn a bunch of small fragments of the earth to go flying off

          // spawn small earth chunks in all possible directions
          // (upleft/upright/dnleft/dnright)
          // up/left/down/right
          for(i=0;i<=9;i++)
          {
            o = spawn_object(player[0].x+(16<<CSF), player[0].y, OBJ_EARTHCHUNK);
            objects[o].ai.ray.direction = i;
            if (i > 4)
            {
              objects[o].sprite = EARTHCHUNK_SMALL_DN;
            }
            else
            {
              objects[o].sprite = EARTHCHUNK_SMALL_UP;
            }
          }

          break;
          case 6:
            o = spawn_object(player[0].x+(16<<CSF), player[0].y+(16<<CSF), OBJ_EXPLOSION);
          break;
          case 7: o = spawn_object(player[0].x+(24<<CSF), player[0].y-(8<<CSF), OBJ_EXPLOSION); break;
          case 8: o = spawn_object(player[0].x+(16<<CSF), player[0].y+(4<<CSF), OBJ_EXPLOSION); break;
          case 10:
          // spawn four big fragments of the earth to go flying off
            o = spawn_object(player[0].x+(16<<CSF), player[0].y, OBJ_EARTHCHUNK);
            objects[o].sprite = EARTHCHUNK_BIG_UP;
            objects[o].ai.ray.direction = EC_UPLEFT;
            o = spawn_object(player[0].x+(16<<CSF), player[0].y, OBJ_EARTHCHUNK);
            objects[o].sprite = EARTHCHUNK_BIG_UP;
            objects[o].ai.ray.direction = EC_UPRIGHT;
            o = spawn_object(player[0].x+(16<<CSF), player[0].y, OBJ_EARTHCHUNK);
            objects[o].sprite = EARTHCHUNK_BIG_DN;
            objects[o].ai.ray.direction = EC_DOWNRIGHT;
            o = spawn_object(player[0].x+(16<<CSF), player[0].y, OBJ_EARTHCHUNK);
            objects[o].sprite = EARTHCHUNK_BIG_DN;
            objects[o].ai.ray.direction = EC_DOWNLEFT;
          break;
          case 32:
            state = TAN_STATE_GAMEOVER;
            sound_play(SOUND_GAME_OVER, PLAY_NOW);
            SetGameOver();
          break;
         }
         spawnedcount++;
         timer = 60;
      }
      else timer--;
    break;
    case TAN_STATE_GAMEOVER:
    break;
    }

    if (fadecontrol.dir==FADE_OUT && fadecontrol.mode==FADE_COMPLETE)
    {  // we're done
      return 0;
    }

    enter = (keytable[KENTER]||keytable[KCTRL]||keytable[KALT]);
    if (enter && !lastenterstate && state==TAN_STATE_GAMEOVER)
    {
      if (fadecontrol.dir!=FADE_OUT)
      {
        fadecontrol.dir = FADE_OUT;
        fadecontrol.curamt = PAL_FADE_SHADES;
        fadecontrol.fadetimer = 0;
        fadecontrol.rate = FADE_NORM;
        fadecontrol.mode = FADE_GO;
      }
    }
    lastenterstate = enter;

    gamedo_fades();
    if (state!=TAN_STATE_GAMEOVER) gamedo_AnimatedTiles();

    gamedo_enemyai();
	//temphack
if (TimeDrv_TimeToRender())
		{
			gamedo_RenderScreen();
			poll_events();
		}
		
    if(((player[0].x>>CSF)-scroll_x) > 160-16) map_scroll_right();
    if (((player[0].y>>CSF)-scroll_y) > 100)
    {
      map_scroll_down();
    }

  } while(!keytable[KESC]);
  return 1;
}

void eseq2_vibrate(void)
{
int xamt, yamt;
int xdir, ydir;
int vibratetimes, waittimer;
int enter,lastenterstate;
int i;
int x,y,w,h;

  vibratetimes = 0;

  #define VIBRATE_AMT             16
  #define VIBRATE_NUM_FRAMES      500

  xamt = yamt = 0;
  xdir = 0; ydir = 0;
  do
  {

    /* undo the scroll from last time */
    if (!xdir)
    {
      for(i=0;i<xamt;i++) map_scroll_right();
    }
    else
    {
      for(i=0;i<xamt;i++) map_scroll_left();
    }

    if (!ydir)
    {
      for(i=0;i<yamt;i++) map_scroll_down();
    }
    else
    {
      for(i=0;i<yamt;i++) map_scroll_up();
    }

    /* randomize a new amount to scroll this frame */
    xamt = (rnd()%VIBRATE_AMT);
    yamt = (rnd()%VIBRATE_AMT);
    xdir = rnd() & 1;
    ydir = rnd() & 1;

    /* scroll the map */
    if (xdir)
    {
      for(i=0;i<xamt;i++) map_scroll_right();
    }
    else
    {
      for(i=0;i<xamt;i++) map_scroll_left();
    }

    if (ydir)
    {
      for(i=0;i<yamt;i++) map_scroll_down();
    }
    else
    {
      for(i=0;i<yamt;i++) map_scroll_up();
    }

    // align sprites with new scroll position
    /*for(i=0;i<MAX_OBJECTS;i++)
    {
      if (objects[i].exists && objects[i].type!=OBJ_PLAYER)
      {
        objects[i].scrx = (objects[i].x>>CSF)-scroll_x;
        objects[i].scry = (objects[i].y>>CSF)-scroll_y;
      }
    }*/
	
    /* show the frame */
	//temphack
		if (TimeDrv_TimeToRender())
		{
			gamedo_RenderScreen();
			poll_events();
		}
    vibratetimes++;

  } while(!keytable[KESC] && vibratetimes < VIBRATE_NUM_FRAMES);

  // display the "uh-oh."
  x = GetStringAttribute("EP2_AfterVibrateString", "LEFT");
  y = GetStringAttribute("EP2_AfterVibrateString", "TOP");
  w = GetStringAttribute("EP2_AfterVibrateString", "WIDTH");
  h = GetStringAttribute("EP2_AfterVibrateString", "HEIGHT");

  eseq_showmsg(getstring("EP2_AfterVibrateString"),x,y,w,h,1);
}


#define HEADFOREARTH_X          0
#define HEADFOREARTH_Y          0

#define HEADFOREARTH_WAIT_TIME  600

#define SPR_SHIP_RIGHT_EP2      132
#define SPR_SHIP_LEFT_EP2       133
#define SPR_VORTICON_MOTHERSHIP 72

#define HEADSFOREARTH_X        3
#define HEADSFOREARTH_Y        15
#define HEADSFOREARTH_W        33
#define HEADSFOREARTH_H        8

void eseq_showmsg(char *text, int boxleft, int boxtop, int boxwidth, int boxheight, char autodismiss)
{
int draw;
int i;

char tempbuf[1024];
char textline, showtimer;
int amountshown;
int waittimer;
int cancel, lastcancelstate;
//temphack
/*
  textline = 0;
  amountshown = 0;
  showtimer = 0;
  lastcancelstate = 1;
  waittimer = 0;

  do
  {
    gamedo_fades();
    gamedo_AnimatedTiles();
    //gamedo_render_drawobjects();

    cancel = (keytable[KENTER]||keytable[KCTRL]||keytable[KALT]);

    // draw the text up to the amount currently shown
    strcpy(tempbuf, text);
    tempbuf[amountshown] = 0;
    sb_dialogbox(boxleft,boxtop,boxwidth,boxheight);
    sb_font_draw(tempbuf, (boxleft+1)*8, (boxtop+1+textline)*8);

    //gamedo_frameskipping_blitonly();
    //gamedo_render_eraseobjects();

    if (showtimer > LETTER_SHOW_SPD)
    {  // it's time to show a new letter
      if (amountshown < strlen(text))
      {
        amountshown++;
      }
      showtimer = 0;
    } else showtimer++;

    // user pressed enter or some other key
    if (cancel && !lastcancelstate)
    {  
      if (amountshown < strlen(text))
      {
         amountshown = strlen(text);
      }
      else return;
    }

    // when all text is shown wait a sec then return
    if (autodismiss)
    {
      if (amountshown >= strlen(text))
      {
        if (waittimer > HEADFOREARTH_WAIT_TIME) return;
        waittimer++;
      }
    }


    lastcancelstate = cancel;

  } while(!keytable[KESC]);
  return;*/
}

int eseq2_HeadsForEarth(void)
{
char enter,lastenterstate;
int x, y;
int downtimer;
int afterfadewaittimer;

  initgame();

  // set up the ship's route
  ShipQueuePtr = 0;
  addshipqueue(CMD_WAIT, 10, 0);
  addshipqueue(CMD_MOVE, 2360, DDOWNRIGHT);
  addshipqueue(CMD_FADEOUT, 0, 0);

  showmapatpos(81, HEADFOREARTH_X, HEADFOREARTH_Y, 0);

  numplayers = 1;
  // place the player near the vorticon mothership
  if (map_findtile(593, &x, &y))
  {
    player[0].x = ((x<<4)+2)<<CSF; 
    player[0].y = ((y<<4)-24)<<CSF; 
  }
  else
  {
      crash("eseq2_HeadForEarth(): unable to find Vorticon Mothership.");
      return 1;
  }

  player[0].playframe = SPR_SHIP_RIGHT_EP2;

  ShipQueuePtr = 0;
  max_scroll_x = max_scroll_y = 20000;

  fadecontrol.mode = FADE_GO;
  fadecontrol.rate = FADE_NORM;
  fadecontrol.dir = FADE_IN;
  fadecontrol.curamt = 0;
  fadecontrol.fadetimer = 0;
  eseq_showmsg(getstring("EP2_ESEQ_PART1"),HEADSFOREARTH_X,HEADSFOREARTH_Y,HEADSFOREARTH_W,HEADSFOREARTH_H,1);

  // erase the message dialog
  map_redraw();

  lastenterstate = 1;
  downtimer = 0;
  afterfadewaittimer = 0;
  do
  {
    // execute the current command in the queue
    if (fadecontrol.dir != FADE_OUT)
    {
         switch(shipqueue[ShipQueuePtr].cmd)
         {
           case CMD_MOVE:
           // down-right only, here
             player[0].x+=9;
             player[0].y+=4;
             // we need a little bit more resolution that we can get--
             // the Y speed needs to be somewhere between 4 and 5 for
             // him to end up at the center of the earth
             if (downtimer > 6)
             {
               player[0].y++;
               downtimer = 0;
             }
             else downtimer++;
           break;
           case CMD_WAIT:
           break;
           case CMD_FADEOUT:
             if (fadecontrol.dir!=FADE_OUT)
             {
                fadecontrol.dir = FADE_OUT;
                fadecontrol.curamt = PAL_FADE_SHADES;
                fadecontrol.fadetimer = 0;
                fadecontrol.rate = FADE_NORM;
                fadecontrol.mode = FADE_GO;
             }
           break;
           default: break;
         }
         // decrease the time remaining
         if (shipqueue[ShipQueuePtr].time)
         {
           shipqueue[ShipQueuePtr].time--;
         }
         else
         {  // no time left on this command, go to next cmd
           ShipQueuePtr++;
         }
    }

    if (fadecontrol.dir==FADE_OUT && fadecontrol.mode==FADE_COMPLETE)
    { 
      if (afterfadewaittimer > 80)
      {
        return 0;
      }
      else afterfadewaittimer++;
    }

    enter = (keytable[KENTER]||keytable[KCTRL]||keytable[KALT]);
    if (enter && !lastenterstate)
    {
      if (fadecontrol.dir!=FADE_OUT)
      {
        fadecontrol.dir = FADE_OUT;
        fadecontrol.curamt = PAL_FADE_SHADES;
        fadecontrol.fadetimer = 0;
        fadecontrol.rate = FADE_NORM;
        fadecontrol.mode = FADE_GO;
      }
    }
    lastenterstate = enter;

    gamedo_fades();
    gamedo_AnimatedTiles();

    //gamedo_frameskipping();
    //gamedo_ScrollT(0);

  } while(!keytable[KESC]);
  return 1;
}

#define LIMPSHOME_X          0
#define LIMPSHOME_Y          344

int eseq2_LimpsHome(void)
{
char enter,lastenterstate;
int x, y;
int downtimer;
int afterfadewaittimer;

  initgame();

  // set up the ship's route
  ShipQueuePtr = 0;
  addshipqueue(CMD_WAIT, 10, 0);
  addshipqueue(CMD_MOVE, 1600, DUPLEFT);
  addshipqueue(CMD_FADEOUT, 0, 0);

  showmapatpos(81, LIMPSHOME_X, LIMPSHOME_Y, 0);

  numplayers = 1;
  player[0].x = (10 <<4<<CSF);
  player[0].y = (26 <<4<<CSF);

  player[0].playframe = SPR_VORTICON_MOTHERSHIP;

  ShipQueuePtr = 0;

  fadecontrol.mode = FADE_GO;
  fadecontrol.rate = FADE_NORM;
  fadecontrol.dir = FADE_IN;
  fadecontrol.curamt = 0;
  fadecontrol.fadetimer = 0;
  eseq_showmsg(getstring("EP2_ESEQ_PART2"),HEADSFOREARTH_X,HEADSFOREARTH_Y,HEADSFOREARTH_W,HEADSFOREARTH_H-1,1);

  // erase the message dialog
  map_redraw();

  lastenterstate = 1;
  downtimer = 0;
  do
  {
    // execute the current command in the queue
    if (fadecontrol.dir != FADE_OUT)
    {
         switch(shipqueue[ShipQueuePtr].cmd)
         {
           case CMD_MOVE:
           // up-left only, here
             player[0].x-=1;
             player[0].y-=2;
           break;
           case CMD_WAIT:
           break;
           case CMD_FADEOUT:
             if (fadecontrol.dir!=FADE_OUT)
             {
               fadecontrol.dir = FADE_OUT;
               fadecontrol.curamt = PAL_FADE_SHADES;
               fadecontrol.fadetimer = 0;
               fadecontrol.rate = FADE_NORM;
               fadecontrol.mode = FADE_GO;
             }
           break;
           default: break;
         }
         // decrease the time remaining
         if (shipqueue[ShipQueuePtr].time)
         {
           shipqueue[ShipQueuePtr].time--;
         }
         else
         {  // no time left on this command, go to next cmd
           ShipQueuePtr++;
         }
    }

    if (fadecontrol.dir==FADE_OUT && fadecontrol.mode==FADE_COMPLETE)
    {
      if (afterfadewaittimer > 80)
      {
        return 0;
      }
      else afterfadewaittimer++;
    }

    enter = (keytable[KENTER]||keytable[KCTRL]||keytable[KALT]);
    if (enter && !lastenterstate)
    {
      if (fadecontrol.dir!=FADE_OUT)
      {
        fadecontrol.dir = FADE_OUT;
        fadecontrol.curamt = PAL_FADE_SHADES;
        fadecontrol.fadetimer = 0;
        fadecontrol.rate = FADE_NORM;
        fadecontrol.mode = FADE_GO;
      }
    }
    lastenterstate = enter;

    gamedo_fades();
    gamedo_AnimatedTiles();

    //gamedo_frameskipping();

  } while(!keytable[KESC]);
  return 1;
}

int eseq2_SnowedOutside(void)
{
int curpage;
int lastpage;
char tempstr[80];
char *text;
int enter, lastenterstate;
int dlgX, dlgY, dlgW, dlgH;

  scrollx_buf = scroll_x = 0;
  scrolly_buf = scroll_y = 0;
  finale_draw("finale.ck2");

  curpage = 1;
  fadecontrol.mode = FADE_GO;
  fadecontrol.rate = FADE_NORM;
  fadecontrol.dir = FADE_IN;
  fadecontrol.curamt = 0;
  fadecontrol.fadetimer = 0;

  numplayers = 1;
  player[0].x = player[0].y = 0;
  player[0].playframe = BLANKSPRITE;

  do
  {
     sprintf(tempstr, "EP2_ESEQ_PART3_PAGE%d", curpage);
     text = getstring(tempstr);
     dlgX = GetStringAttribute(tempstr, "LEFT");
     dlgY = GetStringAttribute(tempstr, "TOP");
     dlgW = GetStringAttribute(tempstr, "WIDTH");
     dlgH = GetStringAttribute(tempstr, "HEIGHT");
     lastpage = GetStringAttribute(tempstr, "LASTPAGE");

     eseq_showmsg(text, dlgX, dlgY, dlgW, dlgH, 1);
     if (lastpage==1) break;

     curpage++;
  } while(!keytable[KESC]);

  finale_draw("finale.ck2");
  //eseq_ToBeContinued();
}
