// extension module to support 256-color graphics in CloneKeen
#include "keen.h"
#include "vgatiles.fdh"
//#include "tga.c"

extern int palette_ncolors;

// load any 256-color sprites we can find, and patch them into
// the existing 16-color sprite set (overwriting any 16-color sprites).
char LoadVGASprites(void)
{
int i;
char fname[MAXPATHLEN];
int changed_player = 0;
	lprintf("Loading 256-color sprites from TGA files...\n");
	
	for(i=0;i<MAX_SPRITES;i++)
	{
		sprintf(fname, "sprite%d.tga", i);
		if (!LoadTGASprite(fname, i))
		{
			if (i >= 0 && i <= 48)
			{
				changed_player = 1;
			}
		}
	}
	
	// need to re-generate alternate player colors?
	if (changed_player)
	{
		createpsprites();
	}
	
	lprintf("LoadVGASprites: palette now at %d colors.\n", palette_ncolors);
	return 0;
}

// load any 256-color tiles we can find, and patch them into
// the existing 16-color tileset (overwriting any 16-color tiles).
char LoadVGATiles(void)
{
int i;
char fname[MAXPATHLEN];
uchar *image, *base;
int x,y;
int w,h;
uchar r,g,b,a;
int c;

	for(i=0;i<MAX_TILES;i++)
	{
		sprintf(fname, "%stile%d.tga", GFXDIR, i);
		if (!LoadTGA(fname, &image, &w, &h))
		{
			if (w != 16 || h != 16)
			{
				free(image);
				crash("LoadVGATiles: image %s is not 16x16 (it's %dx%d)!\n", fname, w,h);
				return 1;
			}
			
			lprintf("Parse %s\n", fname);
			base = image;
			for(y=h-1;y>=0;y--)
			for(x=0;x<w;x++)
			{
				b = *image++; g = *image++; r = *image++; a = *image++;
				// note how transparent pixels are allowed to "show through"
				// to the original 16-color tile
				if (a & 128)
				{		
					c = pal_getcolor(r, g, b);
					if (c==-1) c = pal_addcolor(r, g, b);
					if (c==-1) { crash("LoadVGATiles: out of colorspace"); return 1; }
					
					tiledata[i][y][x] = c;
				}
			}
			free(base);
		}
	}
	
	lprintf("LoadVGATiles: palette now at %d colors.\n", palette_ncolors);
	return 0;
}
