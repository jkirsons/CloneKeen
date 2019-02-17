
#include "keen.h"
#include "graphics.fdh"


void sb_setpixel(int x, int y, unsigned char c)
{
  scrollbuf[(y<<SCROLLBUF_SHIFTAMT) + x] = c;
}

unsigned char sb_getpixel(int x, int y)
{
  return scrollbuf[(y<<SCROLLBUF_SHIFTAMT) + x];
}

// draw a tile directly to the display (bypass the scroll buffer)
void drawtile(int x, int y, unsigned int t)
{
unsigned char xa,ya;
	if (!options[OPT_ZOOM])
	{
	  for(ya=0;ya<16;ya++)
	   for(xa=0;xa<16;xa++)
	    setpixel(x+xa, y+ya, tiledata[t][ya][xa]);
	}
	else
	{
	  x <<= 1; y <<= 1;
	  for(ya=0;ya<32;ya++)
	   for(xa=0;xa<32;xa++)
	   {
	     setpixel(x+xa, y+ya, tiledata[t][ya>>1][xa>>1]);
	   }
	}
}

void sb_drawtile(int x, int y, unsigned int t)
{
int ya;
unsigned char *offset = &scrollbuf[(y<<(SCROLLBUF_SHIFTAMT))+x];
	
	for(ya=0;ya<16;ya++)
	{
		memcpy(offset, &tiledata[t][ya][0], 16);
		offset += SCROLLBUF_XSIZE;
	}
}


// draw a sprite at 1X, using the passed mask as a transparency bitmap
// instead of the mask that comes with the sprite
void drawsprite1x_mask(int xoff, int yoff, unsigned int snum, uchar mask[MAX_SPRITE_HEIGHT][MAX_SPRITE_WIDTH])
{
int x,y,startx,starty,endx,endy;

	if (!sprites[snum].xsize || !sprites[snum].ysize) return;
	
	// clip sprite to screen bounds
	if (xoff<-sprites[snum].xsize) return;
	else if (xoff<0) startx = -xoff;
	else startx = 0;
	
	if (yoff<-sprites[snum].ysize) return;
	else if (yoff<0) starty = -yoff;
	else starty = 0;
	
	if (xoff >= WINDOW_WIDTH)
		return;
	else if (xoff+sprites[snum].xsize >= WINDOW_WIDTH)
		endx = WINDOW_WIDTH - xoff;
	else endx = sprites[snum].xsize;
	
	if (yoff >= WINDOW_HEIGHT)
		return;
	else if (yoff+sprites[snum].ysize >= WINDOW_HEIGHT)
		endy = WINDOW_HEIGHT - yoff;
	else endy = sprites[snum].ysize;
	
	for(y=starty;y<endy;y++)
	 for(x=startx;x<endx;x++)
	 {
       if (mask[y][x])
	     setpixel(x+xoff,y+yoff,sprites[snum].imgdata[y][x]);
	 }

}


// draw a sprite at 2X, using the passed mask as a transparency bitmap
// instead of the mask that comes with the sprite
void drawsprite2x_mask(int xoff, int yoff, unsigned int snum, uchar mask[MAX_SPRITE_HEIGHT][MAX_SPRITE_WIDTH])
{
int x,y,xa,ya,startx,starty,endx,endy,sprsizex,sprsizey;

	if (!sprites[snum].xsize || !sprites[snum].ysize) return;
	sprsizex = (sprites[snum].xsize<<1);
	sprsizey = (sprites[snum].ysize<<1);
	
	// clip sprite to screen bounds
	if (xoff<-sprsizex) return;
	else if (xoff<0) startx = -xoff;
	else startx = 0;
	
	if (yoff<-sprsizey) return;
	else if (yoff<0) starty = -yoff;
	else starty = 0;
	
	if (xoff >= WINDOW_2X_WIDTH)
		return;
	else if (xoff+sprsizex >= WINDOW_2X_WIDTH)
		endx = WINDOW_2X_WIDTH - xoff;
	else endx = sprsizex;
	
	if (yoff >= WINDOW_2X_HEIGHT)
		return;
	else if (yoff+sprsizey >= WINDOW_2X_HEIGHT)
		endy = WINDOW_2X_HEIGHT - yoff;
	else endy = sprsizey;
	
	for(y=starty;y<endy;y++)
	 for(x=startx;x<endx;x++)
	 {
	   ya = y>>1; xa = x>>1;
       if (mask[ya][xa])
	     setpixel(x+xoff,y+yoff,sprites[snum].imgdata[ya][xa]);
	 }
}


void drawsprite(int x, int y, unsigned int snum)
{
	if (options[OPT_ZOOM])
	{
		drawsprite2x(x<<1, y<<1, snum);
	}
	else
	{
		drawsprite1x(x, y, snum);
	}
}

// draws at sprite (at 1x or 2x as appropriate), but does not clip it
// to playfield boundaries. Used by the editor. Awful code but speed
// is not relevant here.
void drawsprite_noclip(int xoff, int yoff, unsigned int s)
{
int x,y,xa,ya;
	for(y=0;y<sprites[s].ysize<<options[OPT_ZOOM];y++)
	{
		for(x=0;x<sprites[s].xsize<<options[OPT_ZOOM];x++)
		{
			if (options[OPT_ZOOM]) { ya = y/2; xa = x/2; } else { ya=y; xa=x; }
			if (sprites[s].maskdata[ya][xa])
				setpixel(x+xoff,y+yoff,sprites[s].imgdata[ya][xa]);
		}
	}
}

void drawghostsprite2x(int xoff, int yoff, unsigned int snum)
{
int x,y,xa,ya,startx,starty,endx,endy,sprsizex,sprsizey;

	if (!sprites[snum].xsize || !sprites[snum].ysize) return;
	sprsizex = (sprites[snum].xsize<<1);
	sprsizey = (sprites[snum].ysize<<1);
	
	// clip sprite to screen bounds
	if (xoff<-sprsizex) return;
	else if (xoff<0) startx = -xoff;
	else startx = 0;
	
	if (yoff<-sprsizey) return;
	else if (yoff<0) starty = -yoff;
	else starty = 0;
	
	if (xoff >= WINDOW_2X_WIDTH)
		return;
	else if (xoff+sprsizex >= WINDOW_2X_WIDTH)
		endx = WINDOW_2X_WIDTH - xoff;
	else endx = sprsizex;
	
	if (yoff >= WINDOW_2X_HEIGHT)
		return;
	else if (yoff+sprsizey >= WINDOW_2X_HEIGHT)
		endy = WINDOW_2X_HEIGHT - yoff;
	else endy = sprsizey;
	
	for(y=starty;y<endy;y++)
	 for(x=startx;x<endx;x++)
	 {
	   ya = y>>1; xa = x>>1;
       if (sprites[snum].maskdata[ya][xa] && ((xa&1)^(ya&1)))
	     setpixel(x+xoff,y+yoff,8);
	 }
}


char LoadSpecialSprites(void)
{
char err;

	// load special clonekeen-specific sprites from the .raw files
	err = 0;
	err |= LoadTGASprite("100.tga", PT100_SPRITE);
	err |= LoadTGASprite("200.tga", PT200_SPRITE);
	err |= LoadTGASprite("500.tga", PT500_SPRITE);
	err |= LoadTGASprite("1000.tga", PT1000_SPRITE);
	err |= LoadTGASprite("5000.tga", PT5000_SPRITE);
	err |= LoadTGASprite("shotup.tga", SHOTUP_SPRITE);
	err |= LoadTGASprite("gunup.tga", GUNUP_SPRITE);
	err |= LoadTGASprite("yorpshield.tga", YORPSHIELD_SPRITE);
	err |= LoadTGASprite("cklogo1.tga", TITLE_LOGO1_SPRITE);
	err |= LoadTGASprite("cklogo2.tga", TITLE_LOGO2_SPRITE);
	err |= LoadTGASprite("version.tga", VERSION_SPRITE);
	err |= LoadTGASprite("demobox.tga", DEMOBOX_SPRITE);
	err |= LoadTGASprite("arrowlr.tga", ARROWLR_SPRITE);
	err |= LoadTGASprite("arrowud.tga", ARROWUD_SPRITE);
	err |= LoadTGASprite("arrowul.tga", ARROWUL_SPRITE);
	err |= LoadTGASprite("arrowur.tga", ARROWUR_SPRITE);
	err |= LoadTGASprite("arrowu.tga", ARROWU_SPRITE);
	err |= LoadTGASprite("arrowd.tga", ARROWD_SPRITE);
	
	if (err)
		crash("LoadSpecialSprites(): Failed to load one or more sprites.\n");
	
	return err;
}

char Graphics_Start(void)
{
	lprintf("Starting graphics driver...\n");
	// tell the video driver (platform-specific) to start up
	if (VidDrv_Start())
	{
		lprintf("Graphics_Start(): VidDrv_Start() failed to initilize display\n");
		return 1;
	}
	
	return 0;
}

void Graphics_Stop(void)
{
  // shut down the video driver
  VidDrv_Stop();
}


void DrawBitmap(int xa, int ya, int b)
{
int x,y,xd,yd;
uchar c;
unsigned char *bmdataptr;

	// for "b" arguments is passed from GetBitmapNumberFromName(),
	// this is in case the specified name was not found
	if (b==-1) return;
	
	bmdataptr = bitmaps[b].bmptr;
	if (!options[OPT_ZOOM])
	{
		for(y=0;y<bitmaps[b].ysize;y++)
		{
			for(x=0;x<bitmaps[b].xsize;x++)
			{
				setpixel(x+xa, y+ya, *(bmdataptr++));
			}
		}
	}
	else
	{
		xa <<= 1; ya <<= 1;
		for(y=yd=0;y<bitmaps[b].ysize;y++,yd+=2)
		{
			for(x=xd=0;x<bitmaps[b].xsize;x++,xd+=2)
			{
				c = *(bmdataptr++);
				setpixel(xd+xa,   yd+ya, c);
				setpixel(xd+xa+1, yd+ya, c);
				setpixel(xd+xa+1, yd+ya+1, c);
				setpixel(xd+xa,   yd+ya+1, c);
			}
		}
	}
}

int GetBitmapNumberFromName(char *bmname)
{
int i;
  for(i=0;i<MAX_BITMAPS;i++)
  {
    bitmaps[i].name[8] = 0;     // ensure null-terminated
    if (!strcmp(bmname, bitmaps[i].name))
    {
      return i;
    }
  }
  return -1;
}


// font drawing functions
