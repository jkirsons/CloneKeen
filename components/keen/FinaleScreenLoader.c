/* FinaleScreenLoader.C
  Code for displaying the FINALE.CK? files.
  Thanks to Andrew Durdin for FIN2BMP, from which I got
  the decompression algorithm.
*/

#include "keen.h"
#include "FinaleScreenLoader.fdh"

int finale_x;
int finale_y;
int finale_count;
int finale_planecol;
int finale_plane_length;
int finale_done;
int y_offset;

// used internally by finale_draw()
void finale_plot(int pix)
{
int mask;

   mask = 128;
   do
   {
     if (pix & mask)
     {
       if (finale_planecol==1)
       {
         sb_setpixel(finale_x, finale_y+y_offset, finale_planecol);
       }
       else
       {  // merge with previous planes
         sb_setpixel(finale_x, finale_y+y_offset, sb_getpixel(finale_x, finale_y+y_offset) | finale_planecol);
       }
     }
     else if (finale_planecol==1)
     {
         sb_setpixel(finale_x, finale_y+y_offset, 0);
     }

     finale_x++;
     if (finale_x > 319)
     {
       finale_x = 0;
       finale_y++;
     }

     finale_count++;
     if (finale_count >= finale_plane_length)
     {
       finale_x = finale_y = 0;
       finale_count = 0;
       finale_planecol <<= 1;
       if (finale_planecol > 8) finale_done = 1;
     }

     if (mask==1)
     {
       return;
     }
     else
     {
       mask >>= 1;
     }

   } while(1);
  
}

// draws a finale.ck? file into the upper-left corner of the scrollbuffer
void finale_draw(char *filename)
{
char fname[256];
FILE *fp;
int cmdbyte;
int bytecount;
int repeatbyte;
int i;
int x,y,by;

	y_offset = (WINDOW_HEIGHT - 200) / 2;
	if (y_offset >= 2) y_offset -= 2;
	
	map_unregister_all_animtiles();
	map.xsize = map.ysize = 16;		// dummy size so gameloop_initialize doesn't bork
	
	sprintf(fname, "data/%s", filename);
	fp = fileopen(fname, "rb");
	if (!fp)
	{
		crash("finale_draw(): cannot open %s file.", filename);
		return;
	}
	lprintf("finale_draw: opened %s\n", filename);
	
	finale_plane_length = fgetl(fp)*2;   //length of a plane when decompressed
	finale_planecol = 1;
	finale_x = 0;
	finale_y = 0;//y_offset;
	finale_count = 0;
	finale_done = 0;
	
	/* decompress/draw the image */
	do
	{
		cmdbyte = __fgetc(fp);
		if (cmdbyte<0)
		{  // EOF
			return;
		}
		
		if (cmdbyte & 0x80)
		{
			//N + 1 bytes of data follows
			bytecount = (cmdbyte & 0x7F) + 1;
			for(i=0;i<bytecount;i++)
			{
				finale_plot(__fgetc(fp));
			}
		}
		else
		{
			//Repeat N + 3 of following byte
			bytecount = (cmdbyte + 3);
			repeatbyte = __fgetc(fp);
			for(i=0;i<bytecount;i++)
			{
				finale_plot(repeatbyte);
			}
		}
	
	} while(!finale_done);
	
	// draw borders
	by = y_offset + 200;
	for(y=0;y<y_offset;y++)
		for(x=0;x<WINDOW_WIDTH;x++)
		{
			sb_setpixel(x, y, 8);
		}
	for(y=by;y<WINDOW_HEIGHT;y++)
		for(x=0;x<WINDOW_WIDTH;x++)
		{
			sb_setpixel(x, y, 8);
		}
	
	__fclose(fp);
	lprintf("finale_draw: finale screen is up\n");
}
