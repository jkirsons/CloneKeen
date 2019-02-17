
#include "keen.h"
#include "graphics.fdh"

void font_draw(unsigned char *text, int xs, int y, void (*drawingfunc)())
{
int i,x,c;
int len = strlen(text);

	x = xs;
	for(i=0;i<len;i++)
	{
		c = text[i];
		if (!c) break;
		if (c != 13)
		{
			(*drawingfunc)(x, y, c);
			x += 8;
		}
		else
		{
			x = xs;
			y += 8;
		}
	}
}


void drawcharacter_clear(int xoff, int yoff, int f)
{
int x,y,xa,ya;
unsigned char c;
  if (f==' ') return;

  if (options[OPT_ZOOM])
  {
		xoff <<= 1;
		yoff <<= 1;
		for(ya=y=0;y<8;y++)
		{
			for(xa=x=0;x<8;x++)
			{
				c = font_clear[f][y][x];
				if (c)
				{
					setpixel(xoff+xa, yoff+ya, c);
					setpixel(xoff+xa+1, yoff+ya, c);
					setpixel(xoff+xa+1, yoff+ya+1, c);
					setpixel(xoff+xa, yoff+ya+1, c);
				}
				xa += 2;
			}
			ya += 2;
		}
  }
  else
  {
		for(y=0;y<8;y++)
			for(x=0;x<8;x++)
				if (font_clear[f][y][x])
					setpixel(xoff+x, yoff+y, font_clear[f][y][x]);
  }
}

// this is used for the flashing yorps left text when you try to exit the level
// without killing all yorps, in must kill all yorps mode.
void drawcharacter_clear_inversepink(int xoff, int yoff, int f)
{
int x,y,xa,ya;
unsigned char c;

  if (f==' ') return;

  if (options[OPT_ZOOM])
  {
		xoff <<= 1;
		yoff <<= 1;
		for(ya=y=0;y<8;y++)
		{
			for(xa=x=0;x<8;x++)
			{
				c = font_clear[f][y][x];
				if (c)
				{
					if (c==15) c = 12; else c = 15;
					setpixel(xoff+xa, yoff+ya, c);
					setpixel(xoff+xa+1, yoff+ya, c);
					setpixel(xoff+xa+1, yoff+ya+1, c);
					setpixel(xoff+xa, yoff+ya+1, c);
				}
				xa += 2;
			}
			ya += 2;
		}
  }
  else
  {
		for(y=0;y<8;y++)
			for(x=0;x<8;x++)
				if (font_clear[f][y][x])
					setpixel(xoff+x, yoff+y, font_clear[f][y][x]);
  }
}

void drawcharacter_clear_erasebg(int xoff, int yoff, int f)
{
int x,y,xa,ya;
unsigned char c;

  if (options[OPT_ZOOM])
  {
		xoff <<= 1;
		yoff <<= 1;
		for(ya=y=0;y<8;y++)
		{
			for(xa=x=0;x<8;x++)
			{
				c = font_clear[f][y][x];
				if (!c) c = 16;
				setpixel(xoff+xa, yoff+ya, c);
				setpixel(xoff+xa+1, yoff+ya, c);
				setpixel(xoff+xa+1, yoff+ya+1, c);
				setpixel(xoff+xa, yoff+ya+1, c);
				xa += 2;
			}
			ya += 2;
		}
  }
  else
  {
		for(y=0;y<8;y++)
			for(x=0;x<8;x++)
				setpixel(xoff+x, yoff+y, font_clear[f][y][x]);
  }
}


void drawcharacter(int xoff, int yoff, int f)
{
int x,y,xa,ya;
unsigned char c;

  if (options[OPT_ZOOM])
  {
		xoff <<= 1;
		yoff <<= 1;
		for(ya=y=0;y<8;y++)
		{
			for(xa=x=0;x<8;x++)
			{
				c = font[f][y][x];
				setpixel(xoff+xa, yoff+ya, c);
				setpixel(xoff+xa+1, yoff+ya, c);
				setpixel(xoff+xa+1, yoff+ya+1, c);
				setpixel(xoff+xa, yoff+ya+1, c);
				xa += 2;
			}
			ya += 2;
		}
  }
  else
  {
		for(y=0;y<8;y++)
			for(x=0;x<8;x++)
				setpixel(xoff+x, yoff+y, font[f][y][x]);
  }
}
