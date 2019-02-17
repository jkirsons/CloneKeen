// having to do with the palette...
#include "keen.h"
#include "palette.fdh"

struct
{
  uchar r[2],g[2],b[2];
} palette[256];
int palette_ncolors = 0;
char isdark = 0;
char palette_dirty = 1;

char pal_load(int episode)
{
int i, dark;
char key[40];
char color[40];
	
	lprintf("Loading palette.ini.\n");
	palette_ncolors = Ini_GetNumericKeyDef("palette.ini", "NCOLORS", 16);
	for(dark=0;dark<=1;dark++)
	{
		for(i=0;i<palette_ncolors;i++)
		{
			sprintf(key, "EP%d%sCOLOR%d", episode, dark?"_DARK_":"_", i);
			if (!Ini_GetKey("palette.ini", key, color))
			{
				if (strlen(color) != 6)
				{
					crash("pal_load: malformatted string '%s' on key '%s' in palette.ini", color, key);
					return 1;
				}
				palette[i].r[dark] = HexToInt(&color[0]);
				palette[i].g[dark] = HexToInt(&color[2]);
				palette[i].b[dark] = HexToInt(&color[4]);
			}
			else
			{
				if (!dark)
				{
					crash("pal_load: Error loading color '%s' from palette.ini", key);
					return 1;
				}
				else
				{
					palette[i].r[dark] = palette[i].r[0];
					palette[i].g[dark] = palette[i].g[0];
					palette[i].b[dark] = palette[i].b[0];
				}
			}
		}
	}
	//for font_clear
	for(dark=0;dark<=1;dark++)
	{
		palette[16].r[dark] = palette[0].r[dark];
		palette[16].g[dark] = palette[0].g[dark];
		palette[16].b[dark] = palette[0].b[dark];
	}
	if (palette_ncolors < 17) palette_ncolors = 17;
	return 0;
}

// sets whether to use the "dark" (lights off) palette or not
void pal_setdark(int dark)
{
	isdark = dark;
}

char fade_black = 0;
void pal_fade(int fadeamt)
{
int c;
int r,g,b;
if (framebyframe) fadeamt = PAL_FADE_SHADES;

	for(c=0;c<palette_ncolors;c++)
	{
		r = palette[c].r[isdark];
		g = palette[c].g[isdark];
		b = palette[c].b[isdark];

		if (fadeamt != PAL_FADE_SHADES)
		{
			if ((c==0 || c==16) && fadeamt > PAL_FADE_SHADES && fade_black)
			{
				r = 255 / (PAL_FADE_WHITEOUT - PAL_FADE_SHADES);
				r = (r * (fadeamt - (PAL_FADE_WHITEOUT - PAL_FADE_SHADES)));
				g = b = r;
			}
			else
			{
				r /= PAL_FADE_SHADES;
				g /= PAL_FADE_SHADES;
				b /= PAL_FADE_SHADES;
				
				r *= fadeamt;
				g *= fadeamt;
				b *= fadeamt;
			}
			
			if (r > 0xff) r = 0xff;
			if (g > 0xff) g = 0xff;
			if (b > 0xff) b = 0xff;
		}

		VidDrv_pal_set(c, r, g, b);
	}
	palette_dirty = 1;
}

// initiate a fade in or fade out
void fade(uchar type, uchar rate)
{

	fadecontrol.fadetimer = 0;
	fadecontrol.mode = FADE_GO;
	fadecontrol.rate = rate;

	// if canceling a fade-out, mark it as completed
	// (unless starting a new fade-out, then the flag will get
	//  reset again just below)
	if (fadecontrol.dir==FADE_OUT && fade_in_progress())
	{
		fadecontrol.fadeout_complete = 1;
	}

	if (type==FADE_OUT)
	{
		fadecontrol.fadeout_complete = 0;
		fadecontrol.dir = FADE_OUT;
		fadecontrol.curamt = PAL_FADE_SHADES;
	}
	else if (type==FADE_IN)
	{
		fadecontrol.dir = FADE_IN;
		fadecontrol.curamt = 0;
		// force a render of the screen in it's current state before a fade-in.
		// otherwise we'll have problems with sometimes fading in to the wrong
		// scene if the render frame timer has triggered yet
		gamedo_RenderScreen();
	}
	else if (type==FADE_FLASH)
	{
		fadecontrol.dir = FADE_FLASH;
		fadecontrol.curamt = PAL_FADE_WHITEOUT;
	}
	else
	{
		crash("fade: invalid fade type %d, rate %d", type, rate);
	}
}


char fade_in_progress(void)		// return whether or not a fade is in progress
{
	return (fadecontrol.mode==FADE_GO);
}

// returns the index of a color in the current palette with an RGB value
// identical to the one given, or -1.
int pal_getcolor(uchar r, uchar g, uchar b)
{
int i;
	for(i=0;i<palette_ncolors;i++)
	{
		if (palette[i].r[isdark]==r &&\
			palette[i].g[isdark]==g &&\
			palette[i].b[isdark]==b)
		{
			//lprintf("pal_getcolor: retn color %d for %d,%d,%d\n", i, r,g,b);
			return i;
		}
	}
	return -1;
}

// adds a color onto the end of the palette and returns it's index.
// if the palette is full, returns -1.
char pal_addcolor(uchar r, uchar g, uchar b)
{
int i;
	if (palette_ncolors >= 256) return -1;
	
	for(i=0;i<=1;i++)
	{
		palette[palette_ncolors].r[i] = r;
		palette[palette_ncolors].g[i] = g;
		palette[palette_ncolors].b[i] = b;
	}
	//lprintf("pal_addcolor: added color %d: %02x%02x%02x\n", palette_ncolors, r,g,b);
	palette_ncolors++;
	return (palette_ncolors-1);
}

