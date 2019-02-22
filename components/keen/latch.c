/* LATCH.C
  This module is reponsible for decoding the EGALATCH and EGASPRIT
  graphics data.
*/

#include "keen.h"
#include "latch.fdh"

unsigned long BitmapBufferRAMSize;

unsigned long getbit_bytepos[5];
unsigned char getbit_bitmask[5];

EgaHead LatchHeader;
SpriteHead *SpriteTable = NULL;
BitmapHead *BitmapTable = NULL;
char *BitmapData;
char *RawData;

// initilizes the positions getbit will retrieve data from
void setplanepositions(unsigned long p1, unsigned long p2, unsigned long p3,\
                       unsigned long p4, unsigned long p5)
{
int i;
	getbit_bytepos[0] = p1;
	getbit_bytepos[1] = p2;
	getbit_bytepos[2] = p3;
	getbit_bytepos[3] = p4;
	getbit_bytepos[4] = p5;
	
	for(i=0;i<=4;i++)
	{
		getbit_bitmask[i] = 128;
	}
}

// retrieves a bit from plane "plane". the positions of the planes
// should have been previously initilized with setplanepositions()
unsigned char getbit(char *buf, unsigned char plane)
{
int retval;
int byt;

	if (!getbit_bitmask[plane])
	{
		getbit_bitmask[plane] = 128;
		getbit_bytepos[plane]++;
	}
	
	byt = buf[getbit_bytepos[plane]];
	retval = (byt & getbit_bitmask[plane]) ? 1:0;
	
	getbit_bitmask[plane] >>= 1;
	
	return retval;
}

// load the EGAHEAD file
char latch_loadheader(int episode)
{
FILE *headfile;
char fname[80];
unsigned long SpriteTableRAMSize;
unsigned long BitmapTableRAMSize;
char buf[12];
int i,j,k;

    sprintf(fname, "data/EGAHEAD.CK%c", episode + '0');
 
    headfile = fileopen(fname, "rb");
    if (!headfile)
    {
      lprintf("latch_loadheader(): unable to open '%s'.\n", fname);
      return 1;
    }

    lprintf("latch_loadheader(): reading main header from '%s'...\n", fname);

    // read the main header data from EGAHEAD
    LatchHeader.LatchPlaneSize = fgetl(headfile);
    LatchHeader.SpritePlaneSize = fgetl(headfile);
    LatchHeader.OffBitmapTable = fgetl(headfile);
    LatchHeader.OffSpriteTable = fgetl(headfile);
    LatchHeader.Num8Tiles = fgeti(headfile);
    LatchHeader.Off8Tiles = fgetl(headfile);
    LatchHeader.Num32Tiles = fgeti(headfile);
    LatchHeader.Off32Tiles = fgetl(headfile);
    LatchHeader.Num16Tiles = fgeti(headfile);
    LatchHeader.Off16Tiles = fgetl(headfile);
    LatchHeader.NumBitmaps = fgeti(headfile);
    LatchHeader.OffBitmaps = fgetl(headfile);
    LatchHeader.NumSprites = fgeti(headfile);
    LatchHeader.OffSprites = fgetl(headfile);
    LatchHeader.Compressed = fgeti(headfile);

    lprintf("   LatchPlaneSize = 0x%04x\n", LatchHeader.LatchPlaneSize);
    lprintf("   SpritePlaneSize = 0x%04x\n", LatchHeader.SpritePlaneSize);
    lprintf("   OffBitmapTable = 0x%04x\n", LatchHeader.OffBitmapTable);
    lprintf("   OffSpriteTable = 0x%04x\n", LatchHeader.OffSpriteTable);
    lprintf("   Num8Tiles = %d\n", LatchHeader.Num8Tiles);
    lprintf("   Off8Tiles = 0x%04x\n", LatchHeader.Off8Tiles);
    lprintf("   Num32Tiles = %d\n", LatchHeader.Num32Tiles);
    lprintf("   Off32Tiles = 0x%04x\n", LatchHeader.Off32Tiles);
    lprintf("   Num16Tiles = %d\n", LatchHeader.Num16Tiles);
    lprintf("   Off16Tiles = 0x%04x\n", LatchHeader.Off16Tiles);
    lprintf("   NumBitmaps = %d\n", LatchHeader.NumBitmaps);
    lprintf("   OffBitmaps = 0x%04x\n", LatchHeader.OffBitmaps);
    lprintf("   NumSprites = %d\n", LatchHeader.NumSprites);
    lprintf("   OffSprites = 0x%04x\n", LatchHeader.OffSprites);
    lprintf("   Compressed = %d\n", LatchHeader.Compressed);

    /** read in the sprite table **/

    // allocate memory for the sprite table
    SpriteTableRAMSize = sizeof(SpriteHead) * (LatchHeader.NumSprites + 1);
    lprintf("latch_loadheader(): Allocating %d bytes for sprite table.\n", SpriteTableRAMSize);

	SpriteTable = malloc(SpriteTableRAMSize);
	if (!SpriteTable)
	{
		lprintf("latch_loadheader(): Can't allocate sprite table!\n");
		__fclose(headfile);
		return 1;
	}

    lprintf("latch_loadheader(): Reading sprite table from '%s'...\n", fname);

    __fseek(headfile, LatchHeader.OffSpriteTable, SEEK_SET);
    for(i=0;i<LatchHeader.NumSprites;i++)
    {
		SpriteTable[i].Width = fgeti(headfile) * 8;
		SpriteTable[i].Height = fgeti(headfile);
		SpriteTable[i].OffsetDelta = fgeti(headfile);
		SpriteTable[i].OffsetParas = fgeti(headfile);
		SpriteTable[i].Rx1 = (fgeti(headfile) >> 8);
		SpriteTable[i].Ry1 = (fgeti(headfile) >> 8);
		SpriteTable[i].Rx2 = (fgeti(headfile) >> 8);
		SpriteTable[i].Ry2 = (fgeti(headfile) >> 8);
		for(j=0;j<16;j++) SpriteTable[i].Name[j] = __fgetc(headfile);
		// for some reason each sprite occurs 4 times in the table.
		// we're only interested in the first occurance.
		for(j=0;j<3;j++)
		{
			for(k=0;k<sizeof(SpriteHead);k++)
				__fgetc(headfile);
		}
	}

    /** read in the bitmap table **/

    // allocate memory for the bitmap table
    BitmapTableRAMSize = sizeof(BitmapHead) * (LatchHeader.NumBitmaps + 1);
    lprintf("latch_loadheader(): Allocating %d bytes for bitmap table.\n", BitmapTableRAMSize);

    BitmapTable = malloc(BitmapTableRAMSize);
	if (!BitmapTable)
	{
		lprintf("latch_loadheader(): Can't allocate bitmap table!\n");
		__fclose(headfile);
		return 1;
	}

    lprintf("latch_loadheader(): reading bitmap table from '%s'...\n", fname);

    __fseek(headfile, LatchHeader.OffBitmapTable, SEEK_SET);

	BitmapBufferRAMSize = 0;
	for(i=0;i<LatchHeader.NumBitmaps;i++)
	{
		BitmapTable[i].Width = fgeti(headfile) * 8;
		BitmapTable[i].Height = fgeti(headfile);
		BitmapTable[i].Offset = fgetl(headfile);
		for(j=0;j<8;j++) BitmapTable[i].Name[j] = __fgetc(headfile);
		
		// keep a tally of the bitmap sizes so we'll know how much RAM we have
		// to allocate for all of the bitmaps once they're decoded
		BitmapBufferRAMSize += (BitmapTable[i].Width * BitmapTable[i].Height);
		
		// print the bitmap info to the console for debug
		for(j=0;j<8;j++) buf[j] = BitmapTable[i].Name[j];
		buf[j] = 0;
		lprintf("   Bitmap '%s': %dx%d at offset %04x. RAMAllocSize=0x%04x\n", buf,BitmapTable[i].Width,BitmapTable[i].Height,BitmapTable[i].Offset,BitmapBufferRAMSize);
	}
	BitmapBufferRAMSize++;

    __fclose(headfile);
    return 0;
}

char latch_loadlatch(int episode)
{
FILE *latchfile;
unsigned long plane1, plane2, plane3, plane4;
char fname[80];
int x,y,t,b,c,p;
char *bmdataptr;
unsigned long RawDataSize;

    sprintf(fname, "data/EGALATCH.CK%c", episode + '0');
 
    lprintf("latch_loadlatch(): Opening file '%s'.\n", fname);

	latchfile = fileopen(fname, "rb");
	if (!latchfile)
	{
		lprintf("latch_loadlatch(): Unable to open '%s'!\n", fname);
		return 1;
	}

    // figure out how much RAM we'll need to read all 4 planes of
    // latch data into memory.
    RawDataSize = (LatchHeader.LatchPlaneSize * 4);
    RawData = malloc(RawDataSize);
    if (!RawData)
    {
		lprintf("latch_loadlatch(): Unable to allocate RawData buffer!\n");
		return 1;
    }

    // get the data out of the file into memory, decompressing if necessary.
    if (LatchHeader.Compressed)
    {
			int ok;
			lprintf("latch_loadlatch(): Decompressing...\n");
			__fseek(latchfile, 6, SEEK_SET);
			ok = lz_decompress(latchfile, RawData);
			if (ok) { 
				lprintf("lzd returns %d\n", ok); 
				__fclose(latchfile);
				return 1; 
			}
		//if (lz_decompress(latchfile, RawData)) return 1;
    }
    else
    {
		lprintf("latch_loadlatch(): Reading %d bytes...\n", RawDataSize);
		__fread(RawData, RawDataSize, 1, latchfile);
    }
    __fclose(latchfile);

    // these are the offsets of the different video planes as
    // relative to each other--that is if a pixel in plane1
    // is at N, the byte for that same pixel in plane3 will be
    // at (N + plane3).
    plane1 = 0;
    plane2 = (LatchHeader.LatchPlaneSize * 1);
    plane3 = (LatchHeader.LatchPlaneSize * 2);
    plane4 = (LatchHeader.LatchPlaneSize * 3);

    // ** read the 8x8 tiles **
    lprintf("latch_loadlatch(): Decoding 8x8 tiles...\n", fname);

    // set up the getbit() function
    setplanepositions(plane1 + LatchHeader.Off8Tiles, \
                      plane2 + LatchHeader.Off8Tiles, \
                      plane3 + LatchHeader.Off8Tiles, \
                      plane4 + LatchHeader.Off8Tiles, \
                      0);

    for(p=0;p<4;p++)
    {
		for(t=0;t<LatchHeader.Num8Tiles;t++)
		{
			for(y=0;y<8;y++)
			{
				for(x=0;x<8;x++)
				{
					// if we're on the first plane start with black,
					// else merge with the previously accumulated data
					if (p==0)
					{
						c = 0;
					}
					else
					{
						c = font[t][y][x];
					}
					
					// read a bit out of the current plane, shift it into the
					// correct position and merge it
					c |= (getbit(RawData, p) << p);
					if (p==3 && !c) c=16;
					font[t][y][x] = c;
				}
			}
		}
    }
	Make_Font_Clear();

    // ** read the 16x16 tiles **
    lprintf("latch_loadlatch(): Decoding 16x16 tiles...\n", fname);

    // set up the getbit() function
    setplanepositions(plane1 + LatchHeader.Off16Tiles, \
                      plane2 + LatchHeader.Off16Tiles, \
                      plane3 + LatchHeader.Off16Tiles, \
                      plane4 + LatchHeader.Off16Tiles, \
                      0);

	for(p=0;p<4;p++)
	{
		for(t=0;t<LatchHeader.Num16Tiles;t++)
		{
			for(y=0;y<16;y++)
			{
				for(x=0;x<16;x++)
				{
					c = (p>0) ? tiledata[t][y][x] : 0;
					c |= (getbit(RawData, p) << p);
					tiledata[t][y][x] = c;
				}
			}
		}
	}
	
	// clear all unused tiles
	for(t=LatchHeader.Num16Tiles;t<MAX_TILES;t++)
	{
		for(y=0;y<TILE_H;y++)
		for(x=0;x<TILE_W;x++)
		{
			tiledata[t][y][x] = ((x&1) ^ (y&1)) ? 8:0;
		}
	}

    // ** read the bitmaps **
    lprintf("latch_loadlatch(): Allocating %d bytes for bitmap data...\n", BitmapBufferRAMSize);
    BitmapData = malloc(BitmapBufferRAMSize);
    if (!BitmapData)
    {
      lprintf("Cannot allocate memory for bitmaps.\n");
      return 1;
    }

    lprintf("latch_loadlatch(): Decoding bitmaps...\n", fname);

    // set up the getbit() function
    setplanepositions(plane1 + LatchHeader.OffBitmaps, \
                      plane2 + LatchHeader.OffBitmaps, \
                      plane3 + LatchHeader.OffBitmaps, \
                      plane4 + LatchHeader.OffBitmaps, \
                      0);

    // decode bitmaps into the BitmapData structure. The bitmaps are
    // loaded into one continous stream of image data, with the bitmaps[]
    // array giving pointers to where each bitmap starts within the stream.

    for(p=0;p<4;p++)
    {
		// this points to the location that we're currently
		// decoding bitmap data to
		bmdataptr = &BitmapData[0];
			
		for(b=0;b<LatchHeader.NumBitmaps;b++)
		{
			bitmaps[b].xsize = BitmapTable[b].Width;
			bitmaps[b].ysize = BitmapTable[b].Height;
			bitmaps[b].bmptr = bmdataptr;
			memcpy(&bitmaps[b].name[0], &BitmapTable[b].Name[0], 8);
			bitmaps[b].name[8] = 0;  //ensure null-terminated
			
			for(y=0;y<bitmaps[b].ysize;y++)
			{
				for(x=0;x<bitmaps[b].xsize;x++)
				{
					if (p==0)
					{
						c = 0;
					}
					else
					{
						c = *bmdataptr;
					}
					c |= (getbit(RawData, p) << p);
					*bmdataptr = c;
					bmdataptr++;
				}
			}
		}
    }

    free(RawData);
    return 0;
}

char latch_loadsprites(int episode)
{
FILE *spritfile;
unsigned long plane1, plane2, plane3, plane4, plane5;
char fname[80];
int x,y,s,p, c;
unsigned long RawDataSize;

    sprintf(fname, "data/EGASPRIT.CK%c", episode + '0');
    lprintf("latch_loadsprites(): Opening file '%s'.\n", fname);
	spritfile = fileopen(fname, "rb");
	if (!spritfile)
	{
		lprintf("latch_loadsprites(): Unable to open '%s'!\n", fname);
		__fclose(spritfile);
		return 1;
    }

    RawDataSize = (LatchHeader.SpritePlaneSize * 5);
    RawData = malloc(RawDataSize);
    if (!RawData)
    {
		lprintf("latch_loadlatch(): Unable to allocate RawData buffer!\n");
		__fclose(spritfile);
		return 1;
    }

    if (LatchHeader.Compressed)
    {
			lprintf("latch_loadsprites(): Decompressing...\n");
			__fseek(spritfile, 6, SEEK_SET);
			if (lz_decompress(spritfile, RawData)) {
				__fclose(spritfile);
				return 1;
			}
    }
    else
    {
		lprintf("latch_loadsprites(): Reading %d bytes...\n", RawDataSize);
		__fread(RawData, RawDataSize, 1, spritfile);
    }
    __fclose(spritfile);

	/*{
		uint checksum = 0;
		for(y=0;y<RawDataSize;y++)
		{
			uint bit;
			checksum += RawData[y];
			bit = checksum & 0x80000000;
			checksum <<= 1;
			checksum |= (bit ? 1:0);
		}
		lprintf("checksum = %d\n", checksum);
		exit(1);
	}*/
    // these are the offsets of the different video planes as
    // relative to each other--that is if a pixel in plane1
    // is at N, the byte for that same pixel in plane3 will be
    // at (N + plane3).
    plane1 = 0;
    plane2 = (LatchHeader.SpritePlaneSize * 1);
    plane3 = (LatchHeader.SpritePlaneSize * 2);
    plane4 = (LatchHeader.SpritePlaneSize * 3);
    plane5 = (LatchHeader.SpritePlaneSize * 4);

    // ** read the sprites **
    lprintf("latch_loadsprites(): Decoding sprites...\n", fname);

    // set up the getbit() function
    setplanepositions(plane1 + LatchHeader.OffSprites, \
                      plane2 + LatchHeader.OffSprites, \
                      plane3 + LatchHeader.OffSprites, \
                      plane4 + LatchHeader.OffSprites, \
                      plane5 + LatchHeader.OffSprites);

    // load the image data
	for(p=0;p<4;p++)
	{
		for(s=0;s<LatchHeader.NumSprites;s++)
		{
			if(sprites[s].imgdata)
			{
				free2D(sprites[s].imgdata, sprites[s].ysize, sprites[s].xsize);
				free2D(sprites[s].maskdata, sprites[s].ysize, sprites[s].xsize);
			}
			sprites[s].imgdata = alloc2D(SpriteTable[s].Height, SpriteTable[s].Width);
			sprites[s].maskdata = alloc2D(SpriteTable[s].Height, SpriteTable[s].Width);

			sprites[s].xsize = (uchar)SpriteTable[s].Width;
			sprites[s].ysize = (uchar)SpriteTable[s].Height;
			sprites[s].bboxX1 = (SpriteTable[s].Rx1 << CSF);
			sprites[s].bboxY1 = (SpriteTable[s].Ry1 << CSF);
			sprites[s].bboxX2 = (SpriteTable[s].Rx2 << CSF);
			sprites[s].bboxY2 = (SpriteTable[s].Ry2 << CSF);

			for(y=0;y<sprites[s].ysize;y++)
			{
				for(x=0;x<sprites[s].xsize;x++)
				{
					c = (p>0) ? sprites[s].imgdata[y][x] : 0;
					c |= (getbit(RawData, p) << p);
					sprites[s].imgdata[y][x] = c;
				}
			}
		}
	}

    // now load the 5th plane, which contains the sprite masks.
    // note that we invert the mask because our graphics functions
    // use white on black masks whereas keen uses black on white.
    for(s=0;s<LatchHeader.NumSprites;s++)
	{
		for(y=0;y<sprites[s].ysize;y++)
		{
			for(x=0;x<sprites[s].xsize;x++)
			{
				sprites[s].maskdata[y][x] = (1 - getbit(RawData, 4));
			}
		}
	}
	
	free(RawData);
    return 0;
}

char latch_loadgraphics(int episode)
{
int retval = 0;
   SpriteTable = NULL;

   if (latch_loadheader(episode)) { retval = 1; goto abort; }
   if (latch_loadlatch(episode)) { retval = 1; goto abort; }
   if (latch_loadsprites(episode)) { retval = 1; goto abort; }

abort: ;
   if (SpriteTable) free(SpriteTable);
   if (BitmapTable) free(BitmapTable);
   return retval;
}

