/* MAPLOADER.C
  The functions which load CK maps (level files).
*/

// This loader also supports a so-called "Special Region", which in short,
// uses some of the reserved fields in the official map spec as an offset
// to a special region at the end of the file, which allows additional fields
// to be saved, such as a level time limit--things which are supported by
// CloneKeen but weren't by the original game. Levels that have a Special
// Region should be loaded perfectly fine by real Keen and non-CloneKeen-aware
// map editors, only without the extra features they provide.

#include <string.h>
#include "keen.h"
#include "maploader.fdh"


// Defines for which numbers in the object layer will spawn which enemies
// for OBJ_ICECANNON: xoff/yoff is a vector for the ice chunk to be thrown at.
// for all others: xoff/yoff is an offset from the tile position, in pixels,
//				   where the object should be placed.

#define TOPE		(20*4)
#define AUTO		-1

#define NUM_ENEMIES_EP1		10
							// type     xoff yoff sector-effector-type
int enemies_ep1[TOPE] =   { OBJ_YORP,AUTO,AUTO,0,\
							OBJ_GARG,AUTO,AUTO,0,\
							OBJ_VORT,AUTO,AUTO,0,\
							OBJ_BUTLER,AUTO,AUTO,0,\
							OBJ_TANK,AUTO,AUTO,0,\
							OBJ_ICECANNON,1,-1,0,\
							OBJ_ICECANNON,0,-1,0,\
							OBJ_ICECANNON,0,1,0,\
							OBJ_ICECANNON,-1,-1,0,\
							OBJ_ROPE,0,0,0,\
							};

#define NUM_ENEMIES_EP2		7
int enemies_ep2[TOPE] =	  { OBJ_VORT,AUTO,AUTO,0,\
							OBJ_BABY,AUTO,AUTO,0,\
							OBJ_VORTELITE,AUTO,AUTO,0,\
							OBJ_WALKER,AUTO,AUTO,0,\
							OBJ_TANKEP2,0,0,0,\
							OBJ_PLATFORM,0,-3,0,\
							OBJ_SPARK,0,0,0,\
							};

#define NUM_ENEMIES_EP3		18
int enemies_ep3[TOPE] =  { OBJ_VORT,AUTO,AUTO,0,\
						   OBJ_BABY,AUTO,AUTO,0,\
						   OBJ_MOTHER,AUTO,AUTO,0,\
						   OBJ_MEEP,AUTO,AUTO,0,\
						   OBJ_NINJA,AUTO,AUTO,0,\
						   OBJ_FOOB,AUTO,AUTO,0,\
						   OBJ_BALL,AUTO,AUTO,0,\
						   OBJ_JACK,AUTO,AUTO,0,\
						   OBJ_PLATFORM,0,0,0,\
						   OBJ_PLATVERT,0,0,0,\
						   OBJ_VORT,AUTO,AUTO,0,\
						   OBJ_SECTOREFFECTOR,0,0,SE_MORTIMER_SPARK,\
						   OBJ_SECTOREFFECTOR,0,0,SE_MORTIMER_HEART,\
						   OBJ_AUTORAY,0,4,0,\
						   OBJ_AUTORAY_V,4,0,0,\
						   OBJ_SECTOREFFECTOR,0,0,SE_MORTIMER_ARM,\
						   OBJ_SECTOREFFECTOR,0,0,SE_MORTIMER_LEG_LEFT,\
						   OBJ_SECTOREFFECTOR,0,0,SE_MORTIMER_LEG_RIGHT\
						   };

#define HD_XSIZE				0
#define HD_YSIZE				1
#define HD_PLANES				2
#define HD_PLANESIZE			7
#define HD_HAS_SPECIAL_1		8			// CloneKeen-specific
#define HD_HAS_SPECIAL_2		9			// CloneKeen-specific
#define HD_SPECIAL_OFFS_MSB		10			// CloneKeen-specific
#define HD_SPECIAL_OFFS_LSB		11			// CloneKeen-specific
#define HD_PLANES_START			16

#define SPECIAL_ALLOC_SIZE		64			// max bytes to save in Special Region (CloneKeen stuff)
// these are just magic numbers so we can know whether the Special Region
// exists or not.
#define SPECIAL_VALUE_1			0x414b		// 'KA'...
#define SPECIAL_VALUE_2			0x5954		// ...'TY'

// codes inside the special region
#define SPECIALCODE_TIMELIMIT	0xA0
#define SPECIALCODE_FORCESKILL	0xA1
#define SPECIALCODE_FORCEOPTION	0xA2
#define SPECIALCODE_END			0x00



// load a CK map file into the engine, returning nonzero on error.
int loadmap(char *filename)
{
FILE *fp;
ulong maplen;
uint plane_size;
uint *data;
int index;
int x, y;
uint t;
unsigned long special_offs;

	delete_all_objects();
	map_setstartpos(2, 2);
  	NessieObjectHandle = 0;
	levelcontrol.sparks_left = 0;
	
	SpecialRegion_SetDefaults();
	
	if (strcasestr(filename, "LEVEL80.CK"))
	{
		map.isworldmap = 1;
	}
	else
	{
		map.isworldmap = 0;
	}
	
	lprintf("loadmap(): loading map '%s'\n", filename);
	fp = fileopen(filename, "rb");
	if (!fp)
	{
		crash("loadmap(): %s: file not found\n", filename);
		__fclose(fp);
		return 1;
	}
	
	maplen = fgetl(fp);
	lprintf("Map uncompressed length: %d bytes.\n", maplen);
	if (!maplen)
	{
		lprintf(">>> This map says it's 0 bytes long...\n"
				">>> it was probably created by a broken map editor\n");
		__fclose(fp);
		return 1;
	}
	
	data = malloc(maplen + 1);
	if (!data)
	{
		lprintf("loadmap: unable to allocate %d bytes.\n", maplen);
		__fclose(fp);
		return 1;
	}
	
	if (rle_decompress(fp, data, maplen))
	{
		lprintf("loadmap: RLE decompression error.\n");
		__fclose(fp);
		return 1;
	}
	
	map.xsize = data[0];
	map.ysize = data[1];
	if (data[2] != 2)
	{
		lprintf("loadmap(): incorrect number of planes (loader only supports 2)\n");
		__fclose(fp);
		return 1;
	}
	
	lprintf("loadmap(): %s, map dimensions %dx%d\n", filename, map.xsize, map.ysize);
	if (map.xsize > MAP_MAXWIDTH || map.ysize >= MAP_MAXHEIGHT)
	{
		crash("loadmap(): level %s is too big (max width %dx%d)\n", filename, MAP_MAXWIDTH, MAP_MAXHEIGHT);
		__fclose(fp);
		return 1;
	}
	
	plane_size = data[7];
	lprintf("plane size %d bytes\n", plane_size);
	if (plane_size & 1)
	{
		crash("loadmap(): plane size is not even!\n");
		__fclose(fp);
		return 1;
	}
	
	// copy the tile layer into the map
	index = HD_PLANES_START;
	for(y=0;y<map.ysize;y++)
	{
		for(x=0;x<map.xsize;x++)
		{
			map.mapdata[x][y] = data[index++];
		}
	}
	
	// copy the object layer into the map	
	// get index of plane 2, rounding up to the nearest 16 worde boundary (8 words)
	index = roundup((HD_PLANES_START + (plane_size / 2)), 8);
	
	for(y=0;y<map.ysize;y++)
	{
		for(x=0;x<map.xsize;x++)
		{
			t = data[index++];
			if (t==255)
			{
				map_setstartpos(x, y);
			}
			else
			{
				map.objectlayer[x][y] = t;
				if (t)
				{
					if (!map.isworldmap)
					{
						// spawn enemies as appropriate
						AddEnemy(x, y);
					}
					else
					{
						// spawn Nessie at first occurance of her path
						if (levelcontrol.episode==3 && t==NESSIE_PATH)
						{
							if (!NessieObjectHandle)
							{
								NessieObjectHandle = spawn_object(x<<TILE_S<<CSF, y<<TILE_S<<CSF, OBJ_NESSIE);
								objects[NessieObjectHandle].hasbeenonscreen = 1;
							}
						}
						else
						{
							// make completed levels into "done" tiles
							t &= 0x7fff;
							if (t < MAX_LEVELS && levelcontrol.levels_completed[t])
							{
								map.objectlayer[x][y] = 0;
								map.mapdata[x][y] = tiles[map.mapdata[x][y]].chgtile;
							}
						}
					}
				}
			}
		}
	}
	
	// check if the mapfile has a clonekeen-specific special region
	if (data[HD_HAS_SPECIAL_1]==SPECIAL_VALUE_1 && \
		data[HD_HAS_SPECIAL_2]==SPECIAL_VALUE_2)
	{
		lprintf("> level created in CloneKeen Editor!\n");
		special_offs = data[HD_SPECIAL_OFFS_MSB]; special_offs <<= 16;
		special_offs |= data[HD_SPECIAL_OFFS_LSB];
		
		lprintf("Parsing CloneKeen-specific data at %08x\n", special_offs);
		if (parse_special_region(&data[special_offs]))
		{
			lprintf("loadmap(): error parsing special region\n");
			__fclose(fp);
			return 1;
		}
	}
	else
	{
		lprintf("> level NOT created by CloneKeen.\n");
	}
	
	expand_320240();
	map_coat_border();
	map_calc_max_scroll();
	
	lprintf("loadmap(): success!\n");
	free(data);
	__fclose(fp);
	
	return 0;
}


// set default values for every attribute affected by the Special Region.
void SpecialRegion_SetDefaults(void)
{
int i;
	map.hastimelimit = 0;
	map.GameOverOnTimeOut = 0;
	for(i=0;i<NUM_OPTIONS;i++) map.forced_options[i] = OPTION_NOT_FORCED;
}

char static parse_special_region(uint *buf)
{
int index = 0;
uint code, len;
int i,k;
	lprintf("parse_special_region: parsing special region...\n");
	
	do
	{
		code = buf[index++];
		len = buf[index++];
		
		lprintf("Code %02x, contents: ", code);
		for(i=0;i<len;i++)
		{
			lprintf("%02x ", buf[index+i]);
		}
		lprintf("\n");
		
		switch(code)
		{
			case SPECIALCODE_END:
				lprintf("> Found end of special region at index %04x.\n", index);
				return 0;
			
			case SPECIALCODE_TIMELIMIT:
				map.hastimelimit = buf[index];
				map.time_m = buf[index+1];
				map.time_s = buf[index+2];
				map.GameOverOnTimeOut = buf[index+3];
				lprintf("  * Time Limit Information: %s, %02dm:%02ds, GameOver=%s\n", map.hastimelimit?"ON":"OFF", map.time_m, map.time_s, map.GameOverOnTimeOut?"YES":"NO");
				break;
				
			case SPECIALCODE_FORCEOPTION:
				k = buf[index];
				if (k < NUM_OPTIONS && k >= 0 && Is_Acceptable_Force_Option(k))
				{
					map.forced_options[k] = buf[index+1];
					lprintf("  * Force Option %d = %d\n", k, buf[index+1]);
				}
				break;
				
			default:
				lprintf("> Ignoring unknown Special Region code %04x at index %04x\n", code, index);
				break;
		}
		
		index += len;
	} while(1);
}

char Is_Acceptable_Force_Option(int k)
{
	switch(k)
	{
		case OPT_FULLSCREEN:
		case OPT_ZOOM:
		case OPT_ZOOMONRESTART:
		case OPT_CHEATS:
		case OPT_SOUNDOFFINDEMO:
		case OPT_GHOSTING:
			return 0;
	}
	return 1;
}


// decompress up to maxlen bytes of data from level file FP,
// storing it in the buffer 'data'. returns nonzero on error.
char static rle_decompress(FILE *fp, uint *data, int maxlen)
{
uint ch;
uint howmany, what;
int index, runs;

	maxlen /= 2;
	lprintf("map_rle_decompress: decompressing %d words.\n", maxlen);
	
	index = 0;
	runs = 0;
	while(!__feof(fp) && index < maxlen)
	{
		ch = fgeti(fp);
		
		if (ch==0xFEFE)
		{
			howmany = fgeti(fp);
			what = fgeti(fp);
			while(howmany--)
			{
				data[index++] = what;
			}
			runs++;
		}
		else
		{
			data[index++] = ch;
		}
	}
	
	if (index < maxlen)
	{
		crash("map_rle_decompress (at %d): uh-oh, less data exists than spec'd in header.\n", index);
		return 1;
	}
	
	lprintf("map_rle_decompress: decompressed %d words in %d runs.\n", index, runs);
	return 0;
}

// RLE-compress the data in buffer "data", saving it to file FP.
char static rle_compress(FILE *fp, uint *data, int len)
{
uint word, lastword, readword;
int compress_index, run_ahead_index;
int runlength;
int i;

	len /= 2;
	lprintf("map_rle_compress: compressing %d words.\n", len);
	
	compress_index = 0;
	while(compress_index < len)
	{
		// read a worde from the buffer
		readword = data[compress_index];
		
		// is the next worde the same? if so find the length of the run.
		if ((compress_index+1 < len) && data[compress_index+1]==readword)
		{
			// find how long the run is (a run of worde readword)
			run_ahead_index = (compress_index + 1);
			runlength = 1;
			do
			{
				word = data[run_ahead_index];
				
				// the run is over when either the worde is different
				// or run_ahead_index is at the end of the buffer,
				// or runlength is approaching FFFF (max possible RLE run length)
				if (word != readword || run_ahead_index >= len || runlength >= 0xFDFF)
				{
					break;
				}
				run_ahead_index++;
				runlength++;
			} while(1);
			
			// it takes 3 words to code a RLE run, so if the run is less than
			// 3 words, it would actually be smaller if we didn't compress it
			if (runlength <= 3 && readword != 0xfefe)
			{
				// RLE run, but too small to bother with
				for(i=0;i<runlength;i++) fputi(readword, fp);
			}
			else
			{
				// save a RLE run
				fputi(0xfefe, fp);
				fputi(runlength, fp);
				fputi(readword, fp);
			}
			// advance
			compress_index += runlength;
		}
		else
		{
			// next word is different, this is not a run, it's just a single word
			if (readword != 0xfefe)
			{
				fputi(readword, fp);
			}
			else
			{
				// it's a single uncompressed word, which is equal to the RLE marker.
				// delimit it by placing it in a RLE run of length 1.
				fputi(0xfefe, fp);
				fputi(1, fp);
				fputi(readword, fp);
			}
			compress_index++;
		}
	}
}

// reset all objects to the way they were when the level was started.
void resetobjects(void)
{
int x, y;

	delete_all_objects();
	levelcontrol.PlatExtending = 0;
	levelcontrol.sparks_left = 0;
	
	if (!map.isworldmap)
	{
		for(y=0;y<map.ysize;y++)
		{
			for(x=0;x<map.xsize;x++)
			{
				if (map.objectlayer[x][y]) AddEnemy(x, y);
			}
		}
	}
}


// check the object layer at position mx,my, and spawn an enemy as appropriate.
void AddEnemy(int mx, int my)
{
int index, objtype, o;
int xoff, yoff;
int ysize, rndysize;
int numenemies;
int *enemies;

	if (!map.objectlayer[mx][my]) return;
	
	switch(levelcontrol.episode)
	{
		case 1:  enemies = &enemies_ep1[0]; numenemies = NUM_ENEMIES_EP1; break;
		case 2:  enemies = &enemies_ep2[0]; numenemies = NUM_ENEMIES_EP2; break;
		default: enemies = &enemies_ep3[0]; numenemies = NUM_ENEMIES_EP3; break;
	}
	
	// obtain the index into the enemies array
	index = (map.objectlayer[mx][my] - 1);
	if (index >= numenemies) return;	// it's object data, like switches etc
	
	index *= 4;
	objtype = enemies[index];
	if (objtype==-1) return;			// this enemy type doesn't do anything
	
	o = spawn_object(0, 0, objtype);
	
	// get location to put the enemy
	xoff = enemies[index + 1];
	yoff = enemies[index + 2];
	if (objtype == OBJ_ICECANNON)
	{
		objects[o].ai.icechunk.vector_x = xoff;
		objects[o].ai.icechunk.vector_y = yoff;
		xoff = yoff = 8;
	}
	else
	{
		if (xoff==AUTO) xoff = 0;
		if (yoff==AUTO)		// line it up with the floor
		{
			// round it up to nearest even # of tiles, then subtract the
			// rounded amount from the actual amount
			yoff = (sprites[objdefsprites[objtype]].ysize % 16);
		}
	}
	
	objects[o].x = ((mx << 4) + xoff) << CSF;
	objects[o].y = ((my << 4) + yoff) << CSF;
	
	if (objtype==OBJ_SECTOREFFECTOR)
	{
		objects[o].ai.se.type = enemies[index + 3];
	}
	
	// some enemies need to start "active"
	switch(objtype)
	{
		case OBJ_WALKER:
		case OBJ_TANK:
		case OBJ_TANKEP2:
		case OBJ_ICECANNON:
		case OBJ_BALL: case OBJ_JACK:
		case OBJ_PLATFORM:
		case OBJ_PLATVERT:
		case OBJ_AUTORAY: case OBJ_AUTORAY_V:
		case OBJ_SECTOREFFECTOR:
		
			objects[o].hasbeenonscreen = 1;
			break;
			
		case OBJ_SPARK:
			levelcontrol.sparks_left++;
			break;
	}
}

// load a level from the current episode
char loadlevel(int levelno)
{
char mapname[100];

	sprintf(mapname, "data/LEVEL%02d.CK%c", levelno, levelcontrol.episode + '0');
	if (loadmap(mapname)) return 1;
	
	if (levelcontrol.custom_episode_path[0]==0)
	{
		SetStopPoints(levelno);
	}
	
	levelcontrol.curlevel = levelno;
	map_resetscroll();		// also redraws the map
	
	// scroll past the border
	while(scroll_x < 32) map_scroll_right();
	while(scroll_y < 32) map_scroll_down();
	return 0;
}

// set enemy stop-points (invisible barriers they won't cross)
void static SetStopPoints(int lvlnum)
{
	if (levelcontrol.episode==1)
	{
		if (lvlnum==13)
		{
			map.objectlayer[94][13] = ENEMY_STOPPOINT;
			map.objectlayer[113][13] = ENEMY_STOPPOINT;
			map.objectlayer[48][6] = ENEMY_STOPPOINT;
			map.objectlayer[80][5] = ENEMY_STOPPOINT;
			map.objectlayer[87][5] = ENEMY_STOPPOINT;
			map.objectlayer[39][18] = ENEMY_STOPPOINT;
		}
		else if (lvlnum==3 && getoption(OPT_MUSTKILLYORPS))
		{
			map.objectlayer[11][48] = ENEMY_STOPPOINT;
			map.objectlayer[11][47] = ENEMY_STOPPOINT;
			map.objectlayer[12][41] = ENEMY_STOPPOINT;
			map.objectlayer[12][40] = ENEMY_STOPPOINT;
			map.objectlayer[30][41] = ENEMY_STOPPOINT;
			map.objectlayer[30][40] = ENEMY_STOPPOINT;
			map.objectlayer[32][12] = ENEMY_STOPPOINT;
			map.objectlayer[32][11] = ENEMY_STOPPOINT;
			map.objectlayer[17][12] = ENEMY_STOPPOINT;
			map.objectlayer[17][11] = ENEMY_STOPPOINT;
		}
	}
	else if (levelcontrol.episode==3)
	{
		if (lvlnum==6)
		{
			map.objectlayer[40][7] = BALL_NOPASSPOINT;
			map.objectlayer[50][7] = BALL_NOPASSPOINT;
		}
		else if (lvlnum==9)
		{
		    map.objectlayer[45][106] = BALL_NOPASSPOINT;
		}
		else if (lvlnum==4)
		{
			map.objectlayer[94][17] = BALL_NOPASSPOINT;
		}
	}
}

// some levels have no vertical scrolling and are only 200 pixels tall.
// since we use a 320x240 resolution now, these levels need to be "expanded"
// so that there is enough data to fill the screen.
void static expand_320240(void)
{
int x,y,i;
	// subtract the border and find out if it's smaller than the height
	if ((map.ysize - 4)<<TILE_S < WINDOW_HEIGHT)
	{
		map.ysize += 2;
		// move all the tiles down 2 spaces
		for(y=map.ysize-2;y>=0;y--)
		for(x=0;x<map.xsize;x++)
		{
			map.mapdata[x][y+2] = map.mapdata[x][y];
			map.objectlayer[x][y+2] = map.objectlayer[x][y];
		}
		 
		// fill the top two lines with sky
		for(x=2;x<map.xsize-2;x++)
		{
			map.mapdata[x][2] = map.mapdata[x][3] = map.mapdata[3][4];
			map.objectlayer[x][2] = map.objectlayer[x][3] = 0;
		}
		// move all objects down
		for(i=0;i<highest_objslot;i++)
		{
			objects[i].y += (32<<CSF);
		}
		for(i=0;i<numplayers;i++) player[i].y += (32<<CSF);
	}
}

// rounds 'index' up to the nearest multiple of 'nearest'
int roundup(int index, int nearest)
{
	if (index % nearest)
	{
		index /= nearest;
		index *= nearest;
		index += nearest;
	}
	return index;
}

char savemap(char *fname)
{
FILE *fp;
int x,y,ch,i;
int plane_size, total_size;
uint *data;
int index, plane2_indx;
int special_start;

	lprintf("savemap: %s\n", fname);
	
	plane_size = (map.xsize * map.ysize) * 2;			// bytes -> words
	plane2_indx = roundup((HD_PLANES_START + (plane_size / 2)), 8);
	total_size = ((plane2_indx * 2) + plane_size) + SPECIAL_ALLOC_SIZE;
	
	lprintf("plane_size %d bytes\ntotal_size %d bytes\n",plane_size,total_size);
	
	data = malloc(total_size);
	if (!data)
	{
		crash("savemap: Unable to allocate buffer for data.\n");
		return 1;
	}
	
	// copy the tile layer into the buffer
	lprintf("Copying tile layer...\n");
	map_coat_border();
	index = HD_PLANES_START;
	for(y=0;y<map.ysize;y++)
	for(x=0;x<map.xsize;x++)
	{
		data[index++] = map.mapdata[x][y];
	}
	
	// copy the object layer into the buffer
	index = plane2_indx;
	lprintf("Copying object layer (to %d)...\n",index);
	for(y=0;y<map.ysize;y++)
	for(x=0;x<map.xsize;x++)
	{
		ch = map.objectlayer[x][y];
		if (ch==255) ch = 0;
		if (x==(map.startx>>CSF)>>TILE_S && y==(map.starty>>CSF)>>TILE_S)
		{
			ch = 255;
		}
		
		data[index++] = ch;
	}
	
	special_start = index;		// start of the Special Region (CloneKeen-specific)
	
	lprintf("index = %d\nindex_bytes=%d total_size = %d\n", index, index*2, total_size);
	if (index*2 > total_size)
	{
		crash("index>total_size\n");
	}
	
	lprintf("> CloneKeen Special Region Offset: %08x\n", special_start);
	
	memset(data, 0, HD_PLANES_START);
	data[HD_XSIZE] = map.xsize;
	data[HD_YSIZE] = map.ysize;
	data[HD_PLANES] = 2;
	data[HD_PLANESIZE] = plane_size;
	
	// mark that map was saved by CloneKeen, and has a Special Region
	data[HD_HAS_SPECIAL_1] = SPECIAL_VALUE_1;
	data[HD_HAS_SPECIAL_2] = SPECIAL_VALUE_2;
	data[HD_SPECIAL_OFFS_MSB] = (special_start & 0xFFFF0000) >> 16;
	data[HD_SPECIAL_OFFS_LSB] = (special_start & 0xFFFF);
	
	// save the Special Region
	index = special_start;
	data[index++] = SPECIALCODE_TIMELIMIT;
	data[index++] = 4;
	data[index++] = map.hastimelimit;
	data[index++] = map.time_m;
	data[index++] = map.time_s;
	data[index++] = map.GameOverOnTimeOut;
	
	for(i=0;i<NUM_OPTIONS;i++)
	{
		if (map.forced_options[i] != OPTION_NOT_FORCED)
		{
			data[index++] = SPECIALCODE_FORCEOPTION;
			data[index++] = 2;
			data[index++] = i;
			data[index++] = map.forced_options[i];
			lprintf(">>>>wrote option %d forced %d\n", map.forced_options[i]);
		}
	}
	
	data[index++] = SPECIALCODE_END;
	data[index] = 0;
	
	
	// open the file and write the data
	fp = fileopen(fname, "wb");
	if (!fp)
	{
		Console_Msg("Can't open %s.", fname);
		free(data);
		return 1;
	}
	
	fputl(total_size, fp);
	rle_compress(fp, data, total_size);
	
	__fclose(fp);
	free(data);
	
	Console_Msg("Saved '%s'.", fname);
	return 0;
}
