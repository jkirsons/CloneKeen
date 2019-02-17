#include "keen.h"
#include "graphicmaker.fdh"
// In a nutshell, these routines create graphics which didn't exist in
// the original game by modifying/combining pieces together of existing
// graphics.
//
// There's also some stuff in here to extract sounds from the original EXE's,
// and etc.
#define CLEAR		255


void static FontCopyColorSwitch(int from, int to, uchar swap1, uchar swap2)
{
int x,y,c;
// copy a font bitmap from tile from to tile to and swap two colors
	for(y=0;y<8;y++)
	 for(x=0;x<8;x++)
	 {
		c = font_clear[from][y][x];
		if (c==swap1) c = swap2;
		else if (c==swap2) c = swap1;
		font_clear[to][y][x] = c;
	 }
}

// generate the "clear" font bitmaps
// these are used in the menu and the "low-profile" statusbar
// they are basically the regular font in a different colour,
// with an outline around each letter
void Make_Font_Clear(void)
{
int f,x,y,xa,ya,c;
uchar stroke_buffer[8][8];
	lprintf("Make_Font_Clear(): Generating font_clear[] from font[]...\n");
	
	#define FC_FORECOLOR		15
	#define FC_STROKECOLOR		16
	
	for(f=0;f<MAX_FONT;f++)
	{
		if (f < 17)
		{				// make white parts clear
			for(y=0;y<8;y++)
			 for(x=0;x<8;x++)
				if (font[f][y][x]==15)
					{ font_clear[f][y][x] = 0; }
				else
					{ font_clear[f][y][x] = font[f][y][x]; }
		}
		else if (f==OPTION_DISABLED_CHAR)	// a y-centered period for menus
		{
			memset(stroke_buffer, 0, sizeof(stroke_buffer));
			for(y=2;y<8;y++)
			 for(x=0;x<8;x++)
				if (font['.'][y][x]!=15) stroke_buffer[y-2][x] = 1;
			goto strokeme;
		}
		else if (f >= 128 && f < 160)
		{
			for(y=0;y<8;y++)
			 for(x=0;x<8;x++)
				font_clear[f][y][x] = font[f][y][x];
		}
		else			// stroke and add border
		{
			if (f >= 128) c = 4; else c = 16;
			if (f == 17) c = 4;

			for(y=0;y<8;y++)
			 for(x=0;x<8;x++)
			 {
				if (font[f][y][x]==c) stroke_buffer[y][x] = 1;
								 else stroke_buffer[y][x] = 0;
			 }
strokeme: ;
			
			// add the stroke
			for(y=0;y<8;y++)
			{
			 for(x=0;x<8;x++)
			 {
				if (!stroke_buffer[y][x])
				{
					for(ya=-1;ya<2;ya++)
					{
						for(xa=-1;xa<2;xa++)
						{
							if (xa==-1 && x==0 || ya==-1 && y==0) goto onedge;
							if (xa==1 && x==7 || ya==1 && y==7) goto onedge;
							if (stroke_buffer[y+ya][x+xa] && (xa != ya))
							{
								font_clear[f][y][x] = FC_STROKECOLOR;
								goto stroked;
							}
onedge: ;
						}
					}
					font_clear[f][y][x] = 0;	// it's clear here
stroked: ;
				}
				else font_clear[f][y][x] = FC_FORECOLOR;
			 }
		 }
		}
	}
	
	// generate the missing frames for the twirler (menu selector)
	// which for some reason are missing from original keen,
	// so that it can go all the way around without looking goofy
	FontCopyColorSwitch(10, 8, 1, 11);
	FontCopyColorSwitch(11, 7, 1, 11);
}

void CreateTitle(void)
{
int floor_px, spr, x, xa, y;
	// create the floor //
	#define FLOOR_WIDTH			80		// amount of floor to create (/2)
	#define FLOOR_TILE			257		// floor tile to use
	#if (FLOOR_WIDTH > MAX_SPRITE_WIDTH)
		#error "CreateTitle: Floor Width must be <= MAX_SPRITE_WIDTH";
	#endif
	floor_px = 11;
	spr = TITLE_FLOOR_SPRITE;

	if(!sprites[spr].imgdata)
	{
		sprites[spr].imgdata = alloc2D(16, FLOOR_WIDTH);
		sprites[spr].maskdata = alloc2D(16, FLOOR_WIDTH);
	}
	for(x=xa=0;x<FLOOR_WIDTH;x++,xa++)
	{
		for(y=0;y<16;y++)
		{
			sprites[spr].imgdata[y][xa] = tiledata[FLOOR_TILE][y][floor_px];
			sprites[spr].maskdata[y][xa] = 1;
		}
		floor_px = (floor_px + 1) & 15;
	}
	sprites[spr].xsize = FLOOR_WIDTH;
	sprites[spr].ysize = 16;
	
	// copy the vorticon sprite
	#include "ai/vort.h"
	CopySprite(VORT1_WALK_LEFT_FRAME+3, TITLE_VORT_SPRITE);
}

// initilize some misc sprites that involve copying or modifying
// from the loaded keen gfx
char initgplsprites(char episode)
{
int x,y,i;

	lprintf("Creating additional in-game sprites...\n");
    if (episode != 3)
    {
		CopyTileToSprite(DOOR_YELLOW, DOOR_YELLOW_SPRITE, 2, 7);
		CopyTileToSprite(DOOR_RED, DOOR_RED_SPRITE, 2, 7);
		CopyTileToSprite(DOOR_GREEN, DOOR_GREEN_SPRITE, 2, 7);
		CopyTileToSprite(DOOR_BLUE, DOOR_BLUE_SPRITE, 2, 7);
    }
    else
    {
		CopyTileToSprite(DOOR_YELLOW_EP3, DOOR_YELLOW_SPRITE, 2, 7);
		CopyTileToSprite(DOOR_RED_EP3, DOOR_RED_SPRITE, 2, 7);
		CopyTileToSprite(DOOR_GREEN_EP3, DOOR_GREEN_SPRITE, 2, 7);
		CopyTileToSprite(DOOR_BLUE_EP3, DOOR_BLUE_SPRITE, 2, 7);
    }
		
	// create the sprites used on the OSD
	CopySprite(PSTANDFRAME, OSD_LIVES_SPRITE);
	sprites[OSD_LIVES_SPRITE].ysize = 10;
		
	switch(episode)
	{
		case 1:
			CopyTileToSprite(TILE_RAYGUN, OSD_AMMO_SPRITE, 1, 7);
			ReplaceSpriteColor(OSD_AMMO_SPRITE, 12, 11, 0);
			ReplaceSpriteColor(OSD_AMMO_SPRITE, 4, 3, 0);
			ReplaceSpriteColor(OSD_AMMO_SPRITE, 13, 9, 0);
		break;
		
		case 2:
			CopyTileToSprite(TILE_RAYGUN, OSD_AMMO_SPRITE, 1, 7);
			// delete the "X" background
			for(y=0;y<sprites[OSD_AMMO_SPRITE].ysize;y++)
			for(x=0;x<sprites[OSD_AMMO_SPRITE].xsize;x++)
			{
				if (sprites[OSD_AMMO_SPRITE].imgdata[y][x]==15 || \
					sprites[OSD_AMMO_SPRITE].imgdata[y][x]==8)
				{ sprites[OSD_AMMO_SPRITE].maskdata[y][x] = 0; }
			}
			
			// erase the antennas
			sprites[OSD_AMMO_SPRITE].maskdata[2][5] = 0;
			sprites[OSD_AMMO_SPRITE].maskdata[3][5] = 0;
			
			sprites[OSD_AMMO_SPRITE].maskdata[2][14] = 0;
			sprites[OSD_AMMO_SPRITE].maskdata[2][12] = 0;
			sprites[OSD_AMMO_SPRITE].maskdata[1][13] = 0;
			sprites[OSD_AMMO_SPRITE].maskdata[3][13] = 0;
		break;
		
		case 3:
			CopyTileToSprite(189, OSD_AMMO_SPRITE, 1, 7);
			ReplaceSpriteColor(OSD_AMMO_SPRITE, 13, 11, 0);
			ReplaceSpriteColor(OSD_AMMO_SPRITE, 5, 3, 0);
			ReplaceSpriteColor(OSD_AMMO_SPRITE, 8, 1, 0);
		break;
	}
	
	// fix the blue-keycard-with-white-background, which for some reason
	// is messed up and has a sky background instead
	if (episode==3 && levelcontrol.custom_episode_path[0]==0)
	{
		ReplaceTileColor(220, 11, 15);
	}
	
	CreateKeycardRisers(episode);

    // create BLANKSPRITE
    sprites[BLANKSPRITE].xsize = sprites[BLANKSPRITE].ysize = 0;

	if (episode != 3)
	{
		makedarkbgpowerups(episode);
	}
	
	if (episode==2)
	{
		// make the Elder Switch Used tile, a non-animated tile from
		// the same animation as the elder switch tile (the switch
		// is changed to this tile after it's activated).
		CopyTile(TILE_ELDERSWITCH+3, TILE_ELDERSWITCH_USED);
		
		// copy over part of the ice cannons (since they're useless anyway),
		// and insert grey and black tiles which we'll set to be partially
		// solid in tileattr. The intended purpose is to be able to use them
		// as lining for the grey tubes, so that you can walk through a
		// suspended tube.
		for(i=455;i<=458;i++) CopyTile(BG_GREY, i);
		for(i=442;i<=445;i++) CopyTile(BG_DARK_EP2, i);
	}
	
	createpsprites();
	setplayercolors();
	return 0;
}

int pspritebases[PCOLOR_LAST];

void setplayercolors(void)
{
	playerbaseframes[0] = pspritebases[options[OPT_P1COLOR]];
	playerbaseframes[1] = pspritebases[options[OPT_P2COLOR]];
}

// creates the "risers" sprites that appear briefly when you collect a keycard
void CreateKeycardRisers(int episode)
{
static int cardsprites[20] =
	{PTCARDY_SPRITE,14,6,PTCARDR_SPRITE,12,4,PTCARDG_SPRITE,10,2,PTCARDB_SPRITE,9,1,0};
int s,i,x,y,c1,c2;

	#define TEMPTILE	(MAX_TILES-1)
	switch(episode)
	{
		case 1:
			CopyTileToSprite(TILE_KEYYELLOW, PTCARDY_SPRITE, 1, 7);
			CopyTileToSprite(TILE_KEYRED, PTCARDR_SPRITE, 1, 7);
			CopyTileToSprite(TILE_KEYGREEN, PTCARDG_SPRITE, 1, 7);
			CopyTileToSprite(TILE_KEYBLUE, PTCARDB_SPRITE, 1, 7);
		break;
		
		case 2:
			CopyTile(424, TEMPTILE);
			ReplaceTileColorObeyBorder(TEMPTILE, 15, CLEAR);
			CopyTileToSprite(TEMPTILE, PTCARDY_SPRITE, 1, CLEAR);
			
			CopyTile(425, TEMPTILE);
			ReplaceTileColorObeyBorder(TEMPTILE, 15, CLEAR);
			CopyTileToSprite(TEMPTILE, PTCARDR_SPRITE, 1, CLEAR);
			
			CopyTile(426, TEMPTILE);
			ReplaceTileColorObeyBorder(TEMPTILE, 15, CLEAR);
			CopyTileToSprite(TEMPTILE, PTCARDG_SPRITE, 1, CLEAR);
			
			CopyTile(427, TEMPTILE);
			ReplaceTileColorObeyBorder(TEMPTILE, 15, CLEAR);
			CopyTileToSprite(TEMPTILE, PTCARDB_SPRITE, 1, CLEAR);
		break;
		
		case 3:
			CopyTileToSprite(191, PTCARDY_SPRITE, 1, 7);
			CopyTileToSprite(192, PTCARDR_SPRITE, 1, 7);
			CopyTileToSprite(193, PTCARDG_SPRITE, 1, 7);
			CopyTileToSprite(194, PTCARDB_SPRITE, 1, 7);
		break;
	}

	i = 0;
	while(cardsprites[i])
	{
		s = cardsprites[i];
		c1 = cardsprites[i+1];
		c2 = cardsprites[i+2];
	
		// remove the SGA letter from the card.
		for(y=5;y<TILE_H-5;y++)
		for(x=6;x<TILE_W-6;x++)
		  if (sprites[s].imgdata[y][x]==12) sprites[s].imgdata[y][x] = 15;
			
		// add "ribbing" to the card
		sprites[s].imgdata[2][2] = c2;
		sprites[s].imgdata[2][TILE_H-3] = c2;
		sprites[s].imgdata[TILE_W-3][2] = c2;
		sprites[s].imgdata[TILE_W-3][TILE_H-3] = c2;
		for(y=4;y<12;y+=2)
		{
			for(x=1;x<TILE_W-1;x++)
			{
				if (sprites[s].imgdata[y][x]==c1)
					sprites[s].imgdata[y][x] = c2;
			}
		}
		
		
		i += 3;
	}
}

// create alternately-colored player sprites
void createpsprites(void)
{
int i,s;
	
	memset(pspritebases, 0, sizeof(pspritebases));
	
    // create the sprites for player 2
    s = LatchHeader.NumSprites;
    
	pspritebases[PCOLOR_YELGREEN] = s;
	for(i=0;i<48;i++)
	{
		CopySprite(i, s);
		ReplaceSpriteColor(s, 13, 10, 0);
		ReplaceSpriteColor(s, 5, 2, 0);
		ReplaceSpriteColor(s, 9, 14, 8);
		ReplaceSpriteColor(s, 1, 6, 8);
		ReplaceSpriteColor(s, 12, 11, 0);
		ReplaceSpriteColor(s, 4, 3, 0);
		s++;
	}

	pspritebases[PCOLOR_BLUEGREEN] = s;
	for(i=0;i<48;i++)
	{
		CopySprite(i, s);
		ReplaceSpriteColor(s, 12, 6, 0);
		ReplaceSpriteColor(s, 4, 2, 0);
		ReplaceSpriteColor(s, 9, 10, 8);
		ReplaceSpriteColor(s, 1, 2, 8);
		ReplaceSpriteColor(s, 13, 9, 0);
		ReplaceSpriteColor(s, 5, 1, 0);
		s++;
	}

	pspritebases[PCOLOR_RED] = s;
	for(i=0;i<48;i++)
	{
		CopySprite(i, s);
		ReplaceSpriteColor(s, 12, 6, 0);
		ReplaceSpriteColor(s, 4, 6, 0);
		ReplaceSpriteColor(s, 13, 12, 0);
		ReplaceSpriteColor(s, 5, 4, 0);
		ReplaceSpriteColor(s, 9, 12, 8);
		ReplaceSpriteColor(s, 1, 4, 8);
		s++;
	}
	setplayercolors();
}

// creates versions of the powerups which are on a dark background.
// these are not in the episodes 1 & 2 but are fun to have in user levels.
void makedarkbgpowerups(int episode)
{
int i,x,y;
	lprintf("Creating dark-bg powerups...\n");
	
	if (episode==1)
	{
		CopyTile(175, TILE_RAYGUN_DARK);
		CopyTile(190, TILE_KEYYELLOW_DARK);
		CopyTile(191, TILE_KEYRED_DARK);
		CopyTile(192, TILE_KEYGREEN_DARK);
		CopyTile(193, TILE_KEYBLUE_DARK);
		CopyTile(202, TILE_100PTS_DARK);
		CopyTile(201, TILE_200PTS_DARK);
		CopyTile(203, TILE_500PTS_DARK);
		CopyTile(204, TILE_1000PTS_DARK);
		CopyTile(205, TILE_5000PTS_DARK);

		ReplaceTileColorEp1Dark(TILE_500PTS_DARK, 7, 0);
		ReplaceTileColorEp1Dark(TILE_1000PTS_DARK, 7, 0);
	}
	else		// episode 2
	{
		CopyTile(414, TILE_RAYGUN_DARK);
		ReplaceTileColor(TILE_RAYGUN_DARK, 15, CLEAR);
		
		CopyTile(424, TILE_KEYYELLOW_DARK);
		ReplaceTileColorObeyBorder(TILE_KEYYELLOW_DARK, 15, CLEAR);

		CopyTile(425, TILE_KEYRED_DARK);
		ReplaceTileColorObeyBorder(TILE_KEYRED_DARK, 15, CLEAR);

		CopyTile(426, TILE_KEYGREEN_DARK);
		ReplaceTileColorObeyBorder(TILE_KEYGREEN_DARK, 15, CLEAR);

		CopyTile(427, TILE_KEYBLUE_DARK);
		ReplaceTileColorObeyBorder(TILE_KEYBLUE_DARK, 15, CLEAR);

		CopyTile(307, TILE_100PTS_DARK); ReplaceTileColor(TILE_100PTS_DARK, 3, CLEAR);
		CopyTile(306, TILE_200PTS_DARK); ReplaceTileColor(TILE_200PTS_DARK, 3, CLEAR);
		CopyTile(308, TILE_500PTS_DARK); ReplaceTileColorObeyBorder(TILE_500PTS_DARK, 3, CLEAR);
		CopyTile(309, TILE_1000PTS_DARK); ReplaceTileColor(TILE_1000PTS_DARK, 3, CLEAR);
		CopyTile(205, TILE_5000PTS_DARK);
		
		ReplaceTileColor(TILE_5000PTS_DARK, 7, CLEAR);
		ReplaceTileColor(TILE_5000PTS_DARK, 8, CLEAR);
		tiledata[TILE_5000PTS_DARK][1][0] = CLEAR;
		tiledata[TILE_5000PTS_DARK][0][TILE_W-2] = CLEAR;
		tiledata[TILE_5000PTS_DARK][4][3] = 0;
	}
	
	for(i=TILES_DARK_START;i<=TILES_DARK_LAST;i++)
	{
		if (episode==2)		// in ep2 we also have to remove the bg lines
		{
			for(y=0;y<TILE_H;y++)
			for(x=0;x<TILE_W;x++)
			{
				if (tiledata[i][y][x]==CLEAR)
				{
					tiledata[i][y][x] = tiledata[BG_DARK_EP2][y][x];
				}
			}
		}
		else
		{
			if (i != TILE_500PTS_DARK && i != TILE_1000PTS_DARK)
				ReplaceTileColor(i, 7, 0);
		}
	}
}


// duplicates sprite source to dest
void CopySprite(int source, int dest)
{
int x,y;

  sprites[dest].xsize = sprites[source].xsize;
  sprites[dest].ysize = sprites[source].ysize;

	if(!sprites[dest].imgdata)
	{
		sprites[dest].imgdata = alloc2D(sprites[source].ysize, sprites[source].xsize);
		sprites[dest].maskdata = alloc2D(sprites[source].ysize, sprites[source].xsize);
	}

  for(y=0;y<sprites[source].ysize;y++)
  {
    for(x=0;x<sprites[source].xsize;x++)
    {
      sprites[dest].imgdata[y][x] = sprites[source].imgdata[y][x];
      sprites[dest].maskdata[y][x] = sprites[source].maskdata[y][x];
    }
  }
}

// duplicates tile source to dest
void CopyTile(int source, int dest)
{
int x,y;
	for(y=0;y<TILE_H;y++)
		for(x=0;x<TILE_W;x++)
			tiledata[dest][y][x] = tiledata[source][y][x];
}

// copies tile data into a sprite. multiple tiles can be copied into
// a single sprite--they will be stacked up vertically.
void CopyTileToSprite(int t, int s, int ntilestocopy, int transparentcol)
{
	int x,y1,y2;
	if(sprites[s].imgdata)
	{
		free2D(sprites[s].imgdata, sprites[s].ysize, sprites[s].xsize);
		free2D(sprites[s].maskdata, sprites[s].ysize, sprites[s].xsize);		
	}
	sprites[s].imgdata = alloc2D(TILE_H * ntilestocopy, TILE_W);
	sprites[s].maskdata = alloc2D(TILE_H * ntilestocopy, TILE_W);

	sprites[s].xsize = TILE_W;
	sprites[s].ysize = TILE_H * ntilestocopy;
	sprites[s].bboxX1 = sprites[s].bboxY1 = 0;
	sprites[s].bboxX2 = (sprites[s].xsize << CSF);
	sprites[s].bboxY2 = (sprites[s].ysize << CSF);

	for(y1=y2=0;y1<sprites[s].ysize;y1++)
	{
		for(x=0;x<TILE_W;x++)
		{
			sprites[s].imgdata[y1][x] = tiledata[t][y2][x];
			
			if (sprites[s].imgdata[y1][x] != transparentcol)
				sprites[s].maskdata[y1][x] = 1;
			else
				sprites[s].maskdata[y1][x] = 0;
		}
		
		if (++y2 >= TILE_W)
		{
			y2 = 0; t++;
		}
	}
}

// replaces all instances of color find in sprite s with
// color replace, as long as the y is greater than miny
void ReplaceSpriteColor(int s, uchar find, uchar replace, int miny)
{
int x,y;

	for(y=miny;y<sprites[s].ysize;y++)
	{
		for(x=0;x<sprites[s].xsize;x++)
		{
			if (sprites[s].imgdata[y][x]==find)
			{
				sprites[s].imgdata[y][x] = replace;
			}
		}
	}
}

void ReplaceTileColor(int t, uchar find, uchar replace)
{
int x,y;
	for(y=0;y<TILE_H;y++)
	{
		for(x=0;x<TILE_W;x++)
		{
			if (tiledata[t][y][x]==find) tiledata[t][y][x] = replace;
		}
	}
}

void ReplaceTileColorObeyBorder(int t, uchar find, uchar replace)
{
int x,y;
char border;
	for(y=0;y<TILE_H;y++)
	{
		border = 0;
		for(x=0;x<TILE_W;x++)
		{
			if (tiledata[t][y][x]==0) border ^= 1;
			if (!border && tiledata[t][y][x]==find) tiledata[t][y][x] = replace;
		}
	}
}

void ReplaceTileColorEp1Dark(int t, uchar find, uchar replace)
{
int x,y;
uchar c1, c2;
	for(y=0;y<TILE_H;y++)
	{
		for(x=0;x<TILE_W;x++)
		{
			if (x+1 < TILE_W) { c1 = tiledata[t][y][x+1]; } else c1 = 0;
			if (x > 0) { c2 = tiledata[t][y][x-1]; } else c2 = 0;
			
			if (c1 == 15 || c2 == 15 || c1==8 || c2==8 || x == 9)
				{ }
			else
				if (tiledata[t][y][x]==find) tiledata[t][y][x] = replace;
		}
	}
}

