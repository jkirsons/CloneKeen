/* MAP.C
  Functions that deal with the level map. Most notably in here
  you'll find the 4-way scrolling engine.
*/

#include "keen.h"
#include "map.fdh"

unsigned long scroll_x = 0;      // total amount of X scroll
unsigned int scrollx_buf = 0;    // amount the scroll buffer is scrolled(x)
unsigned char scrollpix = 0;     // (0-7) for tracking when to draw a stripe
unsigned int mapx = 0;           // map X location shown at scrollbuffer row 0
unsigned int mapxstripepos = 0;  // X pixel position of next stripe row

unsigned long scroll_y = 0;
unsigned int scrolly_buf = 0;    // amount the scroll buffer is scrolled(y)
unsigned char scrollpixy = 0;    // (0-7) for tracking when to draw a stripe
unsigned int mapy = 0;           // map Y location shown at scrollbuffer column 0
unsigned int mapystripepos = 0;  // Y pixel position of next stripe column

// scrolls the map one pixel right
void map_scroll_right(void)
{
	if (scroll_x >= max_scroll_x) return;

	scroll_x++;
	if(scrollx_buf>=(SCROLLBUF_XSIZE-1)) scrollx_buf=0; else scrollx_buf++;

	scrollpix++;
	if (scrollpix > (TILE_W-1))
	{  // need to draw a new stripe
		map_draw_vstripe(mapxstripepos, mapx + SCROLLBUF_NUMTILESX);
		mapx++;
		mapxstripepos += TILE_W;
		if (mapxstripepos >= SCROLLBUF_XSIZE) mapxstripepos = 0;
		scrollpix = 0;
	}
}

// scrolls the map one pixel left
void map_scroll_left(void)
{
	if (scroll_x <= MIN_SCROLL_X) return;
	scroll_x--;
	
	if (scrollx_buf==0) scrollx_buf = (SCROLLBUF_XSIZE-1);
	else scrollx_buf--;

	if (scrollpix==0)
	{  // need to draw a new stripe
		mapx--;
		if (mapxstripepos == 0)
		{
			mapxstripepos = (SCROLLBUF_XSIZE - TILE_W);
		}
		else
		{
			mapxstripepos -= TILE_W;
		}
		
		map_draw_vstripe(mapxstripepos, mapx);
		scrollpix = (TILE_W-1);
	}
	else
	{
		scrollpix--;
	}
}

void map_scroll_down(void)
{
	if (scroll_y >= max_scroll_y) return;

	scroll_y++;
	if(scrolly_buf>=(SCROLLBUF_YSIZE-1)) scrolly_buf=0; else scrolly_buf++;

	scrollpixy++;
	if (scrollpixy > (TILE_H-1))
	{  // need to draw a new stripe
		map_draw_hstripe(mapystripepos, mapy + SCROLLBUF_NUMTILESY);
		mapy++;
		mapystripepos += TILE_H;
		if (mapystripepos >= SCROLLBUF_YSIZE) mapystripepos = 0;
		scrollpixy = 0;
	}
}

void map_scroll_up(void)
{
	if (scroll_y <= MIN_SCROLL_Y) return;
	scroll_y--;
	
	if (!scrolly_buf) scrolly_buf = (SCROLLBUF_YSIZE-1);
	else scrolly_buf--;

	if (scrollpixy==0)
	{  // need to draw a new stripe
		mapy--;
		if (mapystripepos == 0)
		{
			mapystripepos = (SCROLLBUF_YSIZE - TILE_H);
		}
		else
		{
			mapystripepos -= TILE_H;
		}

		map_draw_hstripe(mapystripepos, mapy);
		scrollpixy = (TILE_H-1);
	}
	else
	{
		scrollpixy--;
	}
}


// draws a vertical stripe from map position mapx to scrollbuffer position x
void map_draw_vstripe(unsigned int x, unsigned int mpx)
{
int i,y,c;
int xt=x>>TILE_S;
  for(y=0;y<SCROLLBUF_NUMTILESY;y++)
  {
      c = map.mapdata[mpx][y+mapy];
      sb_drawtile(x, ((y<<TILE_S)+mapystripepos)&(SCROLLBUF_YSIZE-1), c);

      if (AnimTileInUse[x>>TILE_S][(((y<<TILE_S)+mapystripepos)&(SCROLLBUF_YSIZE-1))>>TILE_S])
      { // we just drew over an animated tile which we must unregister
        animtiles[AnimTileInUse[x>>TILE_S][(((y<<TILE_S)+mapystripepos)&(SCROLLBUF_YSIZE-1))>>TILE_S]].slotinuse = 0;
        AnimTileInUse[x>>TILE_S][(((y<<TILE_S)+mapystripepos)&(SCROLLBUF_YSIZE-1))>>TILE_S] = 0;
      }
      if (tiles[c].isAnimated)
      { // we just drew an animated tile which we will now register
        for(i=1;i<MAX_ANIMTILES-1;i++)
        {
          if (!animtiles[i].slotinuse)
          {  // we found an unused slot
            animtiles[i].x = x;
            animtiles[i].y = (((y<<TILE_S)+mapystripepos)&(SCROLLBUF_YSIZE-1));
            animtiles[i].baseframe = c - tiles[c].animOffset;
            animtiles[i].offset = tiles[c].animOffset;
            animtiles[i].slotinuse = 1;
            AnimTileInUse[x>>TILE_S][((((y<<TILE_S)+mapystripepos)&(SCROLLBUF_YSIZE-1)))>>TILE_S] = i;
            break;
          }
        }
      }
  }
}
// draw a horizontal stripe, for vertical scrolling
void map_draw_hstripe(unsigned int y, unsigned int mpy)
{
int i,x,c;
int xt;
  for(x=0;x<SCROLLBUF_NUMTILESX;x++)
  {
      c = map.mapdata[x+mapx][mpy];
      sb_drawtile(((x<<TILE_S)+mapxstripepos)&(SCROLLBUF_XSIZE-1), y, c);

      if (AnimTileInUse[(((x<<TILE_S)+mapxstripepos)&(SCROLLBUF_XSIZE-1))>>TILE_S][y>>TILE_S])
      { // we just drew over an animated tile which we must unregister
        animtiles[AnimTileInUse[(((x<<TILE_S)+mapxstripepos)&(SCROLLBUF_XSIZE-1))>>TILE_S][y>>TILE_S]].slotinuse = 0;
        AnimTileInUse[(((x<<TILE_S)+mapxstripepos)&(SCROLLBUF_XSIZE-1))>>TILE_S][y>>TILE_S] = 0;
      }
      if (tiles[c].isAnimated)
      { // we just drew an animated tile which we will now register
        for(i=1;i<MAX_ANIMTILES-1;i++)
        {
          if (!animtiles[i].slotinuse)
          {  // we found an unused slot
             animtiles[i].x = ((x<<TILE_S)+mapxstripepos)&(SCROLLBUF_XSIZE-1);
             animtiles[i].y = y;
             animtiles[i].baseframe = c - tiles[c].animOffset;
             animtiles[i].offset = tiles[c].animOffset;
             animtiles[i].slotinuse = 1;
             AnimTileInUse[(((x<<TILE_S)+mapxstripepos)&(SCROLLBUF_XSIZE-1))>>TILE_S][y>>TILE_S] = i;
			 break;
          }
        }
      }
  }
}

// returns the map tile at pixel position (x,y) relative to the upper-left
// corner of the map.
unsigned int getmaptileat(unsigned int x, unsigned int y)
{
unsigned int xa, ya;

	xa = (x >> TILE_S);
	ya = (y >> TILE_S);
	// the check is "<=" on Y, because of the "fell off the map" kill-border
	if (xa<map.xsize && ya<=map.ysize) // also includes negatives cause it's unsigned
	{
		return map.mapdata[xa][ya];
	}
	else
	{
		//crash("getmaptileat(): Out-Of-Bounds reading from mapdata (at map tile %d,%d).", x, y);
	
		return TILE_SOLID;
	}
}
unsigned int getlevelat(unsigned int xpx, unsigned int ypx)
{
  return map.objectlayer[xpx>>TILE_S][ypx>>TILE_S];
}

// called at start of level to draw the upper-left corner of the map
// onto the scrollbuffer...from then on the map will only be drawn
// in stripes as it scrolls around.
void drawmap(void)
{
int y;
    for(y=0;y<SCROLLBUF_NUMTILESY;y++)
    {
      map_draw_hstripe(y<<TILE_S, y);
    }
}

// changes the tile at (x,y) in real time
void map_chgtile(int x, int y, int newtile)
{
int oldtile;

   oldtile = map.mapdata[x][y];
   map.mapdata[x][y] = newtile;

   if (map_tile_visible(x, y))
   {
		if (tiles[oldtile].isAnimated) map_deanimate(x, y);
		sb_drawtile(((mapxstripepos+((x-mapx)<<TILE_S))&(SCROLLBUF_XSIZE-1)), ((mapystripepos+((y-mapy)<<TILE_S))&(SCROLLBUF_YSIZE-1)), newtile);
		if (tiles[newtile].isAnimated) map_animate(x, y);
   }   
}

// returns whether or not tile at map coords [x,y] is currently visible
char map_tile_visible(int x, int y)
{
   return (x>=mapx && y>=mapy && x<mapx+SCROLLBUF_NUMTILESX && y<mapy+SCROLLBUF_NUMTILESY);
}

// searches for animated tiles at the map position (X,Y) and
// unregisters them from animtiles
void map_deanimate(int x, int y)
{
int px,py;
int i;

	// figure out pixel position of map tile (x,y)
	px = ((mapxstripepos+((x-mapx)<<TILE_S))&(SCROLLBUF_XSIZE-1));
	py = ((mapystripepos+((y-mapy)<<TILE_S))&(SCROLLBUF_XSIZE-1));

	// find it!
	for(i=1;i<MAX_ANIMTILES-1;i++)
	{
		if (animtiles[i].x == px && animtiles[i].y == py)
		{
			animtiles[i].slotinuse = 0;
			AnimTileInUse[px>>TILE_S][py>>TILE_S] = 0;
			return;
		}
	}
}

// register a tile as animated (for use with map_chgtile)
void map_animate(int x, int y)
{
int px,py;
int c, i;

	c = map.mapdata[x][y];
	if (!tiles[c].isAnimated)
	{
		crash("map_animate(): you told me to animate tile at [%d,%d] but it's not an animated tile!", x, y);
		return;
	}

	if (!map_tile_visible(x, y)) return;

	// figure out pixel position of map tile (x,y) relative to top of scrollbuffer
	px = ((mapxstripepos+((x-mapx)<<TILE_S))&(SCROLLBUF_XSIZE-1));
	py = ((mapystripepos+((y-mapy)<<TILE_S))&(SCROLLBUF_XSIZE-1));

	// don't reanimate a tile that's already registered--then we'd
	// have multiple entries for it in animtiles[] (that's not good).
	if (AnimTileInUse[px>>TILE_S][py>>TILE_S])
	{
		return;
	}

	// find an unused slot in animtiles
	for(i=1;i<MAX_ANIMTILES-1;i++)
	{
		if (!animtiles[i].slotinuse)
		{  // we found an unused slot
			animtiles[i].x = px;
			animtiles[i].y = py;
			animtiles[i].baseframe = c - tiles[c].animOffset;
			animtiles[i].offset = tiles[c].animOffset;
			animtiles[i].slotinuse = 1;
			AnimTileInUse[px>>TILE_S][py>>TILE_S] = i;
			return;
		}
	}

	crash("Unable to animate tile at [%d,%d]: out of animation slots", x, y);
}

void map_unregister_all_animtiles(void)
{
int i;
  for(i=0;i<MAX_ANIMTILES-1;i++) animtiles[i].slotinuse = 0;
}

// searches the map's object layer for object OBJ.
// if it is found returns nonzero and places the
// coordinates of the first occurance of the object
// in (xout,yout)
char map_findobject(int obj, int *xout, int *yout)
{
int x,y;

  for(y=2;y<map.ysize-2;y++)
  {
    for(x=2;x<map.xsize-2;x++)
    {
      if (map.objectlayer[x][y]==obj)
      {
        *xout = x;
        *yout = y;
        return 1;
      }
    }
  }
  return 0;
}

// searches the map's tile layer for tile TILE.
// if it is found returns nonzero and places the
// coordinates of the first occurance of the tile
// in (xout,yout)
char map_findtile(int tile, int *xout, int *yout)
{
int x,y;

  for(y=2;y<map.ysize-2;y++)
  {
    for(x=2;x<map.xsize-2;x++)
    {
      if (map.mapdata[x][y]==tile)
      {
        *xout = x;
        *yout = y;
        return 1;
      }
    }
  }
  return 0;
}

// refreshes the map at the current scroll position
// (unlike drawmap() which does not honor the scroll and will
// glitch up if scrollx/y is != 0)
void map_redraw(void)
{
int x,mpx;

	x = mapxstripepos;
	for(mpx=0;mpx<32;mpx++)
	{
		map_draw_vstripe(x,mpx+mapx);
		x += 16;
		x &= (SCROLLBUF_XSIZE-1);
	}
}

void map_calc_max_scroll(void)
{
	// calc the maximum amount the map is allowed to scroll
	max_scroll_x = (map.xsize-2-(WINDOW_WIDTH/16))<<TILE_S;
	max_scroll_y = (map.ysize<<TILE_S)-WINDOW_HEIGHT-32;	
}

// coat the map with the 2-tile border found all on original Keen maps
void map_coat_border(void)
{
int x,y,c,border;

	border= (levelcontrol.episode==3)?245:144;
	for(x=0;x<map.xsize;x++)
	{
		map.mapdata[x][0] = border;
		map.mapdata[x][1] = border;
		map.mapdata[x][map.ysize-1] = border;
		map.mapdata[x][map.ysize-2] = border;
	}
	for(y=0;y<map.ysize;y++)
	{
		map.mapdata[0][y] = border;
		map.mapdata[1][y] = border;
		map.mapdata[map.xsize-1][y] = border;
		map.mapdata[map.xsize-2][y] = border;
	}

	if (levelcontrol.episode == 3)
	{
		// coat the top of the map ("oh no!" border) with a non-solid tile
		// so keen can jump partially off the top of the screen
		for(x=2;x<map.xsize-2;x++)
		{
			map.mapdata[x][1] = 143;
		}

		// make it lethal to fall off the bottom of the map.
		for(x=2;x<map.xsize-2;x++)
		{
			map.mapdata[x][map.ysize] = TILE_FELLOFFMAP_EP3;
		}
   }
   else
   {
		// coat the bottom of the map below the border.
		// since the border has solidceil=1 this provides
		// a platform to catch enemies that fall off the map
		for(x=2;x<map.xsize-2;x++)
		{
			map.mapdata[x][map.ysize] = TILE_FELLOFFMAP;
		}
   }		
}

void map_resetscroll(void)
{
	scroll_x = 0; scrollx_buf = 0;
	scrollpix = 0; mapx = 0; mapxstripepos = 0;

	scroll_y = 0; scrolly_buf = 0;
	scrollpixy = 0; mapy = 0; mapystripepos = 0;
	map_redraw();
}

// specify the map tile at which the player will start
// and move all players to their starting positions.
void map_setstartpos(int x, int y)
{
	map.startx = x << TILE_S << CSF;
	map.starty = ((((y << TILE_S) + 32) - sprites[0].ysize)) << CSF;
	game_SetStartPositions();
}
