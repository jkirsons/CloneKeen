#include "keen.h"
#include <stdarg.h>
#include <SDL.h>
#include "tileattr.fdh"
// which version of attribute file this program creates.
#define WRITES_ATTRFILEVERSION		2
#define TA_VERSION				"1.2"

char redraw=1;
int episode;
int selcol = 0;
int seltile = 0;
int maxshown;
int NumTiles = 0;
int tilestartpos;
int editsellasttime = 0;
char attrfilename[256];
char scrollchanged;

int baseframe=0,endframe;
char time_to_animate;


void drawtext(int x, int y, const char *str, ...)
{
	va_list ar;
	char buf[2048];

	va_start(ar, str);
	vsprintf(buf, str, ar);
	va_end(ar);

//               lprintf("writing '%s' to %d,%d\n",buf,x,y);
 // sb_font_draw_inverse(buf, (x-1)*8,(y-1)*8);
	font_draw(buf, (x-1)*8,(y-1)*8, drawcharacter_clear_erasebg);
}

void flipit(void)
{
	// this is also needed because if we never call RunLogic,
	// TimeToRender will think we're behind on logic and never
	// let us render anything.
	time_to_animate = TimeDrv_TimeToRunLogic();

	// render the frame
	if (TimeDrv_TimeToRender())
	{
		VidDrv_flipbuffer();
	}
		poll_events();
}


void plotpixel_50p(int x, int y, uchar c)
{
	if (options[OPT_ZOOM])
	{
		x <<= 1; y <<= 1;
		setpixel(x,y,c);
		setpixel(x+1,y+1,c);
		if (c == 8) c = 0;
		setpixel(x+1,y,c);
		setpixel(x,y+1,c);
	}
	else
	{
		setpixel(x,y,c);
	}
}

void plotpixel(int x, int y, uchar c)
{
	if (options[OPT_ZOOM])
	{
		x <<= 1; y <<= 1;
		setpixel(x,y,c);
		setpixel(x+1,y,c);
		setpixel(x+1,y+1,c);
		setpixel(x,y+1,c);
	}
	else
	{
		setpixel(x,y,c);
	}
}


#define CAN_PASS_COL    8
#define CANT_PASS_COL   15

void savetiles(char *filename)
{
char fname[1024];
FILE *fp;
int t;
		strcpy(fname, levelcontrol.custom_episode_path);
		strcat(fname, filename);
          fp = __fopen(fname, "wb");
		  if (!fp)
		  {
			crash("unable to open: '%s'\n", fname);
			return;
		  }

          // save header
          __fputc('A', fp);
          __fputc('T', fp);
          __fputc('R', fp);
          __fputc(episode, fp);
          __fputc(WRITES_ATTRFILEVERSION, fp);

          for(t=0;t<MAX_TILES-1;t++)
          {
            __fputc(tiles[t].solidl, fp);
            __fputc(tiles[t].solidr, fp);
            __fputc(tiles[t].solidfall, fp);
            __fputc(tiles[t].solidceil, fp);
            __fputc(tiles[t].ice, fp);
            __fputc(tiles[t].semiice, fp);
            __fputc(tiles[t].priority, fp);
            __fputc(tiles[t].masktile, fp);
            __fputc(tiles[t].goodie, fp);
            __fputc(tiles[t].standgoodie, fp);
            __fputc(tiles[t].pickupable, fp);
			fputi(tiles[t].points, fp);
            __fputc(tiles[t].lethal, fp);
            __fputc(tiles[t].bonklethal, fp);
			fputi(tiles[t].chgtile, fp);
            __fputc(tiles[t].isAnimated, fp);
            __fputc(tiles[t].animOffset, fp);
            __fputc(tiles[t].animlength, fp);
          }
          __fclose(fp);
}

#define TILESPACING_X     24
#define TILESPACING_Y     28
int disptiles(int startingtile)
{
int t = startingtile;
int x, y, ntiles;
int xa, ya, c;

  maxshown = 0;
  x = y = 5;
  ntiles = 0;
  do
  {
    if (t < MAX_TILES) { drawtile(x, y, t); } else { drawtile(x, y, 0); }
    if (t > maxshown) maxshown=t;

    c = 0;
    if (tiles[t].semiice) c = 9;
    if (tiles[t].ice) c = 11;
    if (t >= MAX_TILES) c = 0;
    ya = y - 4;
    for(xa=0;xa<20;xa++) plotpixel_50p(x+xa-2, ya, c);

    c = CAN_PASS_COL; if (tiles[t].solidfall) c = CANT_PASS_COL;
    if (t >= MAX_TILES) c = 0;
    ya = y - 2;
    for(xa=0;xa<20;xa++) plotpixel_50p(x+xa-2, ya, c);

    c = CAN_PASS_COL; if (tiles[t].solidceil) c = CANT_PASS_COL;
    if (t >= MAX_TILES) c = 0;
    ya = y + 17;
    for(xa=0;xa<20;xa++) plotpixel_50p(x+xa-2, ya, c);

    c = CAN_PASS_COL; if (tiles[t].solidl) c = CANT_PASS_COL;
    if (t >= MAX_TILES) c = 0;
    for(ya=0;ya<20;ya++)
    {
      plotpixel_50p(x-2, y+ya-2, c);
    }

    c = CAN_PASS_COL; if (tiles[t].solidr) c = CANT_PASS_COL;
    if (t >= MAX_TILES) c = 0;
    for(ya=0;ya<20;ya++)
    {
      plotpixel_50p(x+17, y+ya-2, c);
    }
    
    if (tiles[t].lethal || tiles[t].bonklethal) c = 15; else c = 0;
    if (t >= MAX_TILES) c = 0;
    xa = x - 2;
    ya = y + 19;
    plotpixel(xa,ya,c);
    plotpixel(xa,ya+1,c);
    plotpixel(xa,ya+2,c);
    plotpixel(xa+1,ya+2,c);
    plotpixel(xa+2,ya+2,c);

    if (tiles[t].goodie || tiles[t].standgoodie) c = 15; else c = 0;
    if (t >= MAX_TILES) c = 0;
    xa = x + 2;
    ya = y + 19;
    plotpixel(xa,ya,c);
    plotpixel(xa+1,ya,c);
    plotpixel(xa,ya+1,c);
    plotpixel(xa+1,ya+1,c);
    plotpixel(xa+1,ya+2,c);
    plotpixel(xa,ya+3,c);

    if (tiles[t].pickupable) c = 15; else c = 0;
    if (t >= MAX_TILES) c = 0;
    xa = x + 5;
    ya = y + 19;
    plotpixel(xa,ya,c);
    plotpixel(xa+1,ya,c);
    plotpixel(xa,ya+1,c);
    plotpixel(xa+1,ya+1,c);
    plotpixel(xa,ya+2,c);

    if (tiles[t].priority) c = 15; else c = 0;
    if (t >= MAX_TILES) c = 0;
    xa = x + 8;
    ya = y + 19;
    plotpixel(xa,ya,c);
    plotpixel(xa+1,ya,c);
    plotpixel(xa+2,ya,c);
    plotpixel(xa,ya+1,c);
    plotpixel(xa,ya+2,c);
    plotpixel(xa+1,ya+2,c);
    plotpixel(xa+2,ya+2,c);
    plotpixel(xa,ya+3,c);

//    if (tiles[t].playerpriority[0]) c = 15; else c = 0;
//    xa = x + 11;
//    ya = y + 19;

    if (tiles[t].isAnimated) c = 15; else c = 0;
    if (t >= MAX_TILES) c = 0;
    xa = x + 14;
    ya = y + 19;
    plotpixel(xa,ya,c);
    plotpixel(xa+1,ya,c);
    plotpixel(xa+2,ya,c);
    plotpixel(xa,ya+1,c);
    plotpixel(xa,ya+2,c);
    plotpixel(xa+2,ya+1,c);
    plotpixel(xa+2,ya+2,c);
    plotpixel(xa+2,ya+3,c);
    plotpixel(xa,ya+3,c);
    plotpixel(xa+1,ya+2,c);

    x += TILESPACING_X;

    ntiles++;
    if (ntiles>=13)
    {
      ntiles = 0;
      x = 5;
      y += TILESPACING_Y;
    }
    t++;
  } while(y<140);
}

void drawselection(int startingtile, int hilitetile, int erase)
{
int t = startingtile;
int x, y, ntiles;
int xa, ya, c;

  x = y = 5;
  ntiles = 0;
  do
  {
    if (t==hilitetile)
	{
		c = erase ? 0 : (rand()%14)+1;		
	    // draw the flashing border around the selected tile
	      ya = y - 3;
	      for(xa=0;xa<20;xa++)
	      {
	        plotpixel(x+xa-2, ya, c);
	        plotpixel(x+xa-2, ya + 21, c);
	      }
	      for(ya=0;ya<20;ya++)
	      {
	        plotpixel(x-3, y+ya-2, c);
	        plotpixel(x+18, y+ya-2, c);
	      }
		  break;
	}
	
    x += TILESPACING_X;

    ntiles++;
    if (ntiles>=13)
    {
      ntiles = 0;
      x = 5;
      y += TILESPACING_Y;
    }
    t++;
  } while(y<140);
}


void drawbox(int x1, int y1, int x2, int y2, int c)
{
int x,y;
  for(y=y1;y<=y2;y++)
  for(x=x1;x<=x2;x++)
    plotpixel(x,y,c);
}
void drawrect(int x1, int y1, int x2, int y2, int c)
{
int x,y;
  for(x=x1;x<x2;x++)
  {
    plotpixel(x,y1,c);
    plotpixel(x,y2,c);
  }
  for(y=y1;y<=y2;y++)
  {
    plotpixel(x1,y,c);
    plotpixel(x2,y,c);
  }
}

void edittile(int t)
{
int x,y,i,c,sel;
//ass
char *desc1[80] = {"If set tile will be   ",
                   "If set this tile will ",
				   "How many points you   ",
                   "If set tile is deadly ",
                   "If set will kill the  ",
                   "If set causes player  ",
                   "If set player will    ",
                   "If set this tile will",
                   "Specifies tile to     ",
                   "If set tile is solid  ",
                   "If set tile is solid  ",
                   "If set tile can be    ",
                   "If set tile is solid  ",
                   "If set the next tile  ",
                   "If set keen_get_goodie"
                  };
char *desc2[80] = {"processed by          ",
                   "be changed to it's    ",
				   "get for picking this  ",
                   "to the touch. For this",
                   "player if he bonks his",
                   "to slide across the   ",
                   "have no friction when ",
                   "appear in front of    ",
                   "change to when goodie ",
                   "on it's left side.    ",
                   "on it's right side.   ",
                   "stood upon.           ",
                   "on it's bottom side.  ",
                   "will be used as a mask",
                   "will be called when   "
                  };
char *desc3[80] = {"keen_get_goodie when  ",
                   "chgtile parameter when",
				   "tile up.              ",
                   "flag to work, 'goodie'",
                   "head on it. (Used for ",
                   "surface of the tile.  ",
                   "walking on the tile,  ",
                   "sprites.              ",
                   "is picked up or 'done'",
                   "                      ",
                   "                      ",
                   "                      ",
                   "                      ",
                   "to draw this tile over",
                   "player stands on tile."
                  };
char *desc4[80] = {"the player touches it.",
                   "picked up as a goodie.",
                   "                      ",
                   "must also be set.     ",
                   "hanging moss in ep1)  ",
                   "                      ",
                   "but will not slide.   ",
                   "                      ",
                   "tile on completed lvls",
                   "                      ",
                   "                      ",
                   "                      ",
                   "                      ",
                   "players and objects.  ",
                   "(Cave spikes in ep3). "
                  };

 #define EDIT_W         190
 #define EDIT_H         188
 #define EDIT_X         ((320/2)-(EDIT_W/2))
 #define EDIT_Y         10

 x = 13;
 y = 3;

 drawbox(EDIT_X, EDIT_Y, EDIT_X+EDIT_W, EDIT_Y+EDIT_H, 0);
 drawrect(EDIT_X, EDIT_Y, EDIT_X+EDIT_W, EDIT_Y+EDIT_H, 15);
 drawtile(EDIT_X+4,EDIT_Y+4,t);
 drawtext(x,y,"Tile %d    ",t);

 last_keytable[KENTER] = 1;
 sel = editsellasttime;
 redraw = 1;
 do
 {
	poll_events();
		 x = 13;
         y = 2;

         y += 3;
         #define NUM_SELECTIONS         15
	if (redraw)
	{
         drawtext(x,y,"goodie : %d   ", tiles[t].goodie); y++;
         drawtext(x,y, "pickupable : %d   ", tiles[t].pickupable); y++;
         drawtext(x,y, "points : %d   ", tiles[t].points); y++;
         drawtext(x,y, "lethal : %d   ", tiles[t].lethal); y++;
         drawtext(x,y, "bonklethal : %d   ", tiles[t].bonklethal); y++;
         drawtext(x,y, "ice : %d   ", tiles[t].ice); y++;
         drawtext(x,y, "semiice : %d   ", tiles[t].semiice); y++;
         drawtext(x,y, "priority : %d   ", tiles[t].priority); y++;
         drawtext(x,y, "chgtile : %d   ", tiles[t].chgtile); y++;
         drawtext(x,y, "solidl : %d   ", tiles[t].solidl); y++;
         drawtext(x,y, "solidr : %d   ", tiles[t].solidr); y++;
         drawtext(x,y, "solidfall : %d   ", tiles[t].solidfall); y++;
         drawtext(x,y, "solidceil : %d   ", tiles[t].solidceil); y++;
         drawtext(x,y, "hasmask : %d   ", tiles[t].masktile); y++;
//         drawtext(x,y, "solidceil : %d   ", tiles[t].solidceil); y++;
         drawtext(x,y, "standgoodie : %d   ", tiles[t].standgoodie); y++;
         y = 21;
         x-=3;
         drawtext(x,y, "%s", desc1[sel]); y++;
         drawtext(x,y, "%s", desc2[sel]); y++;
         drawtext(x,y, "%s", desc3[sel]); y++;
         drawtext(x,y, "%s", desc4[sel]); y++;

		drawtile(319-16,200-34,tiles[t].chgtile);
	
	    x = EDIT_X + 11;
	    for(i=0;i<=NUM_SELECTIONS-1;i++)
	    {
	      y = EDIT_Y + 33 + (8 * i) - 8;
	      if (i==sel) c = 10; else c = 8;
	      drawbox(x, y, x + 4, y + 4, c);
	    }

		VidDrv_flipbuffer();
		redraw = 0;
	}
	
	
    if (keytable[KDOWN] && !last_keytable[KDOWN])
    {
      sel++;
      if (sel>NUM_SELECTIONS-1) sel=0;
	  redraw = 1;
    }
    else if (keytable[KUP] && !last_keytable[KUP])
    {
      sel--;
      if (sel<0) sel=NUM_SELECTIONS-1;
	  redraw = 1;
    }
    else if (keytable[KENTER] && !last_keytable[KENTER])
    {
	  redraw = 1;
	  sound_play(SOUND_SWITCH_TOGGLE, PLAY_NOW);
       switch(sel)
       {
         case 0: tiles[t].goodie ^= 1; break;
         case 1:
           tiles[t].pickupable ^= 1;
           tiles[t].goodie = tiles[t].pickupable;
           break;
		 case 2:
			tiles[t].points += 100;
			if (tiles[t].points==1100) tiles[t].points = 5000;
			if (tiles[t].points > 5000) tiles[t].points = 0;
			break;
         case 3:
           tiles[t].lethal ^= 1;
           tiles[t].goodie = tiles[t].lethal;
           break;
         case 4: tiles[t].bonklethal ^= 1; break;
         case 5: tiles[t].ice ^= 1; break;
         case 6: tiles[t].semiice ^= 1; break;
         case 7: tiles[t].priority ^= 1; break;
         case 8:        // done tiles: 77-81
           if (episode==1)
           {
             if(tiles[t].chgtile==77) tiles[t].chgtile=78;
             else if(tiles[t].chgtile==78) tiles[t].chgtile=79;
             else if(tiles[t].chgtile==79) tiles[t].chgtile=80;
             else if(tiles[t].chgtile==80) tiles[t].chgtile=81;
             else if(tiles[t].chgtile==81) tiles[t].chgtile=BG_GREY;
             else if(tiles[t].chgtile==BG_GREY) tiles[t].chgtile=BG_BLACK;
			 else if(tiles[t].chgtile==BG_BLACK) tiles[t].chgtile=YORPSTATUEHEADUSED;
             else if(tiles[t].chgtile==YORPSTATUEHEADUSED) tiles[t].chgtile=GARGSTATUEHEADUSED;
             else tiles[t].chgtile=77;
           }
           else if (episode==2)
           {
             if(tiles[t].chgtile==77) tiles[t].chgtile=78;
             else if(tiles[t].chgtile==78) tiles[t].chgtile=79;
             else if(tiles[t].chgtile==79) tiles[t].chgtile=80;
             else if(tiles[t].chgtile==80) tiles[t].chgtile=81;
             else if(tiles[t].chgtile==81) tiles[t].chgtile=143;
             else if(tiles[t].chgtile==BG_GREY) tiles[t].chgtile=BG_DARK_EP2;
			 else if(tiles[t].chgtile==BG_DARK_EP2) tiles[t].chgtile=305;
             else tiles[t].chgtile=77;
           }
           else if (episode==3)
           {
             if(tiles[t].chgtile==52) tiles[t].chgtile=53;
             else if(tiles[t].chgtile==53) tiles[t].chgtile=54;
             else if(tiles[t].chgtile==54) tiles[t].chgtile=55;
             else if(tiles[t].chgtile==55) tiles[t].chgtile=56;
             else if(tiles[t].chgtile==56) tiles[t].chgtile=143;
             else if(tiles[t].chgtile==143) tiles[t].chgtile=143+13;
             else if(tiles[t].chgtile==143+13) tiles[t].chgtile=143+13+13;
             else if(tiles[t].chgtile==143+13+13) tiles[t].chgtile=143+13+13+13;
             else if(tiles[t].chgtile==143+13+13+13) tiles[t].chgtile=143+13+13+13+13;
             else if(tiles[t].chgtile==143+13+13+13+13) tiles[t].chgtile=143+13+13+13+13+13;
             else if(tiles[t].chgtile==143+13+13+13+13+13) tiles[t].chgtile=0;
             else tiles[t].chgtile=52;
           }
           else printf("Invalid episode %d!\n", episode);
           break; // chgtile
         case 9: tiles[t].solidl ^= 1; break;
         case 10: tiles[t].solidr ^= 1; break;
         case 11: tiles[t].solidfall ^= 1; break;
         case 12: tiles[t].solidceil ^= 1; break;
         case 13: tiles[t].masktile ^= 1; break;
         case 14: tiles[t].standgoodie ^= 1; break;
       }
    }

    if (keytable[KESC] && !last_keytable[KESC]) break;
    memcpy(last_keytable, keytable, sizeof(last_keytable));
 } while(1);

 drawbox(EDIT_X, EDIT_Y, EDIT_X+EDIT_W, EDIT_Y+EDIT_H, 0);
 editsellasttime = sel;
 redraw = 1;
 TimeDrv_InitTimers();
}

void show_animated_tile(void)
{
static int animtiletimer=0, animtileframe=0;

	if (!time_to_animate) return;

	if (tiles[seltile].isAnimated)
	{
		baseframe = seltile - tiles[seltile].animOffset;
		drawtext(1,27,"baseframe:%d len:%d frame:%d         ",baseframe,tiles[seltile].animlength,animtileframe);

		if (animtiletimer > ANIM_TILE_TIME)
		{
			animtileframe++;
			if (animtileframe >= tiles[seltile].animlength)
			{
				animtileframe = 0;
			}
				drawtile(319-16,200-16,baseframe+animtileframe);
				animtiletimer=0;
			}
			else animtiletimer++;
	}
}


void ta_banner(void)
{
  lprintf("*******************************\n");
  lprintf(" Tile Attribute Editor %s SDL\n", TA_VERSION);
  lprintf(" for CloneKeen\n");
  lprintf(" (c)2004-2007 Caitlin Shaw\n");
  lprintf("*******************************\n\n");
}

void handlekeys(void)
{
int len;
int i;

      if (keytable[KRIGHT] && !last_keytable[KRIGHT])
      {
		drawselection(tilestartpos, seltile, 1);
        seltile++;
      }
      else if (keytable[KLEFT] && !last_keytable[KLEFT])
      {
		drawselection(tilestartpos, seltile, 1);
        if (seltile>0) seltile--;
      }
      else if (keytable[KUP] && !last_keytable[KUP])
      {
		drawselection(tilestartpos, seltile, 1);
        if (seltile>=13) seltile-=13;
      }
      else if (keytable[KDOWN] && !last_keytable[KDOWN])
      {      
		drawselection(tilestartpos, seltile, 1);
        seltile += 13;
      }

		// these are W,A,S,D
      else if (keytable[KUP2] && !last_keytable[KUP2])
      {
         tiles[seltile].solidfall ^= 1;
		 redraw = 1;
      }
      else if (keytable[KLEFT2] && !last_keytable[KLEFT2])
      {
         tiles[seltile].solidl ^= 1;
         redraw = 1;
      }
      else if (keytable[KDOWN2] && !last_keytable[KDOWN2])
      {
         tiles[seltile].solidceil ^= 1;
		 redraw = 1;
      }
      else if (keytable[KRIGHT2] && !last_keytable[KRIGHT2])
      {
         tiles[seltile].solidr ^= 1;
         redraw = 1;
      }
      else if (keytable[KF1] && !last_keytable[KF1])
      {
         sprintf(attrfilename, "ep%cattr.dat", episode + '0');
         savetiles(attrfilename);
         drawtext(1,30,"%s saved.                  ",attrfilename);
      }
      else if (keytable[KPLUS] && !last_keytable[KPLUS])
      {
	    redraw = 1;
        if (tiles[seltile].isAnimated)
        {
           // already animated...remove all animation
           len = tiles[seltile].animlength + 1;
           endframe = baseframe+tiles[seltile].animlength;
           for(i=baseframe;i<endframe;i++)
           {
             tiles[i].isAnimated = 0;
             tiles[i].animlength = 0;
             tiles[i].animOffset = 0;
           }
        }
        else
        {
           baseframe = seltile;
           len = 2;
        }
        for(i=baseframe;i<baseframe+len;i++)
        {
          tiles[i].isAnimated = 1;
          tiles[i].animlength = len;
          tiles[i].animOffset = i - baseframe;
        }
      }
      else if (keytable[KMINUS] && !last_keytable[KMINUS])
      {
	    redraw = 1;
        if (tiles[seltile].isAnimated)
        {
           len = tiles[seltile].animlength - 1;
           endframe = baseframe+tiles[seltile].animlength;
           for(i=baseframe;i<=endframe;i++)
           {
             tiles[i].isAnimated = 0;
             tiles[i].animlength = 0;
             tiles[i].animOffset = 0;
           }
           if (len>1)
           {
              for(i=baseframe;i<baseframe+len;i++)
              {
                 tiles[i].isAnimated = 1;
                 tiles[i].animlength = len;
                 tiles[i].animOffset = i - baseframe;
              }
           }
        }
     }
     else if (keytable[KENTER] && !last_keytable[KENTER])
     {
       if (seltile < MAX_TILES) edittile(seltile);
       last_keytable[KESC] = 1;
       scrollchanged = 1;
     }
}
/*
int main(int argc, char **argv)
{
int i, k;
int lastseltile;
uchar c;
char tempbuf[80];

	if (lprintf_init())
	{
		printf("WARNING: lprintf could not be inited\n");
	}
	ta_banner();
	LoadOptions();
	if (0)
	{
		lprintf("Unable to load game options\n");
		return 1;
	}
	options[OPT_FULLSCREEN] = 0;
	options[OPT_ZOOM] = 1;

  memset(&levelcontrol, 0, sizeof(levelcontrol));
	

  episode = 1;
  if (argc>1)
  {
    for(i=1;i<argc;i++)
    {
      strcpy(tempbuf, argv[i]);
      if (!strcmp(tempbuf, "-ep1"))
      {
        episode = 1;
      }
      else if (!strcmp(tempbuf, "-ep2"))
      {
        episode = 2;
      }
      else if (!strcmp(tempbuf, "-ep3"))
      {
        episode = 3;
      }
	  else if (!strcmp(tempbuf, "-custom") && i+1 < argc)
	  {
		strcpy(levelcontrol.custom_episode_path, argv[i+1]);
		c = levelcontrol.custom_episode_path[strlen(levelcontrol.custom_episode_path)-1];
		if (c != '\\' && c != '/')
		{
			strcat(levelcontrol.custom_episode_path, "/");
		}
//		levelcontrol.play_custom_episode = 1;
		i++;
	  }
      else if (!strcmp(tempbuf, "-fs"))
      {
        options[OPT_FULLSCREEN] = 1;
      }
      else if (!strcmp(tempbuf, "-nozoom"))
      {
        options[OPT_ZOOM] = 0;
      }
      else
      {
        lprintf(" ** I don't know what '%s' means.\n", tempbuf);
        lprintf("\nUsage: tileattr [-ep1/-ep2/-ep3] [-fs]\n");
		lprintf("Example: tileattr -ep2\n\n");
		lprintf("Additional options:\n\n");
		lprintf("-nozoom\t\t\tUse 320x240 mode instead of 640x480.\n");
		lprintf("-custom [path]\t\tSpecify path to a custom episode you want to edit.\n\n");
		lprintf("The default episode is 1.");
        goto abort;
      }
    }
  }
  
  if (Game_Startup())
  {
	lprintf("Unable to start game engine.\n");
	return 1;
  }

  	seltile = 169;
	tilestartpos = (seltile-52);

change_episode: ;

	if (Load_Episode(episode))
	{
		lprintf("Unable to load episode %d\n", episode);
		Game_Shutdown();
		return 1;
	}
	

  NumTiles = LatchHeader.Num16Tiles;	
  for(i=0;i<MAX_TILES-1;i++)
  {
    if (tiles[i].masktile) tiles[i].masktile = 1;
  }

  sprintf(tempbuf, "CloneKeen Tile Attribute Editor %s - Episode %d (%d tiles)", TA_VERSION, episode, NumTiles);
  //SDL_WM_SetCaption(tempbuf);
  
  if (NumTiles <= seltile)
  {
	seltile = 169;
	tilestartpos = (seltile-52);
  }

  // so KUP2/KDOWN2 work correctly
  levelcontrol.curlevel = WORLD_MAP;

    pal_load(episode);
	pal_setdark(0);
	pal_fade(PAL_FADE_SHADES);

	lastseltile = -1;
	scrollchanged = 1;
	TimeDrv_InitTimers();
	redraw = 1;
	lprintf("entering main loop.\n");

	drawtext(1,29,"USE W,A,S,D TO SET SOLIDITY   F2-EPISODE");
	sound_play(SOUND_GUN_CLICK, PLAY_NOW);
	do
	{	 
		if (seltile != lastseltile) { redraw = 1; }
		if (scrollchanged) { redraw = 1; scrollchanged = 0; }
		
		if (redraw)
		{
			disptiles(tilestartpos);
			drawtext(1,21,"tile:%d sl:%d sr:%d sfall:%d sceil:%d ",seltile,tiles[seltile].solidl,tiles[seltile].solidr,tiles[seltile].solidfall,tiles[seltile].solidceil);
			drawtext(1,22,"ice:%d semiice:%d lethal:%d bonklethal:%d ",tiles[seltile].ice,tiles[seltile].semiice,tiles[seltile].lethal,tiles[seltile].bonklethal);
			drawtext(1,23,"goodie:%d standgoodie:%d pickupable:%d ",tiles[seltile].goodie,tiles[seltile].standgoodie,tiles[seltile].pickupable);
			drawtext(1,24,"priority:%d hasmask:%d chgtile:%03d ",tiles[seltile].priority,tiles[seltile].masktile,tiles[seltile].chgtile);

			drawtext(29,28,"            ",tiles[seltile].points);
			if (tiles[seltile].points)
			{
				sprintf(tempbuf, "points:%d",tiles[seltile].points);
				drawtext(41-strlen(tempbuf),28, tempbuf);
			}

			drawtext(1,26,"isAnimated:%d offset:%d length:%d ",tiles[seltile].isAnimated,tiles[seltile].animOffset,tiles[seltile].animlength);
			drawtext(1,30,"F1-SAVE ENT-EDIT ESC-QUIT +/- ANIM   ");
			drawtile(319-16,200-34,tiles[seltile].chgtile);

			// erase animated tile info
			drawtext(1,27,"                                     ");
			drawbox(319-16,200-16,319,200,0);

			
			lastseltile = seltile;
			VidDrv_flipbuffer();
			redraw = 0;
		}
		drawselection(tilestartpos, seltile, 0);

		show_animated_tile();		 
		if (++selcol>15) selcol = 0;

		handlekeys();

		if(seltile >= MAX_TILES) seltile = MAX_TILES - 1;
		if(seltile > maxshown) { tilestartpos+=13; scrollchanged = 1; }
		if(seltile < tilestartpos) { tilestartpos-=13; scrollchanged = 1; }

		if (keytable[KESC] && !last_keytable[KESC]) break;

		if (keytable[KF2] && !last_keytable[KF2])
		{
			episode++;
			if (episode > 3) episode = 1;
			last_keytable[KF2] = 1;
			sound_play(SOUND_SWITCH_TOGGLE, PLAY_NOW);
			goto change_episode;
		}
		
		memcpy(last_keytable, keytable, sizeof(last_keytable));
     
		flipit();	 
	} while(!crashflag);

fatal: ;
cleanup: ;
  Game_Shutdown();
  ta_banner();
  return 0;

abort: ;
  return 1;
}
*/