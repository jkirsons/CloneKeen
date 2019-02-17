// Targa image loader, included from "vgatiles.c"
#include "tga.h"

char LoadTGA(char *file, uchar **image, int *widthout, int *heightout)
// load a 32-bit uncompressed RGBA targa file, and return a pointer to
// the raw image data. The width and height of the image are returned as well.
{
TGA_HEADER header;
FILE *fp;
int bytesperpixel;
ulong imgdatasize;

	if (!(fp=__fopen(file, "rb")))
	{
		return 1;
	}

	// read the header
	header.identsize = __fgetc(fp);
	__fgetc(fp);
	header.imagetype = __fgetc(fp);
	fgeti(fp); fgeti(fp); __fgetc(fp);
	
	header.xstart = fgeti(fp);
	header.ystart = fgeti(fp);
	header.width = fgeti(fp);
	header.height = fgeti(fp);
	header.bpp = __fgetc(fp);
	__fgetc(fp);
	
	if (header.imagetype != TGA_RGB)
	{
		crash("\nLoadTGA: %s: imagetype must be RGBA uncompressed!\n", file);
		__fclose(fp);
		return 1;
	}
	if (header.bpp != 32)
	{
		crash("\nLoadTGA: %s: image bpp must be 32 (RGB w/ alpha channel)\n", file);
		__fclose(fp);
		return 1;
	}

	*widthout = header.width;
	*heightout = header.height;
	
	bytesperpixel = (header.bpp / 8);
	imgdatasize = header.width * header.height * bytesperpixel;
	*image = malloc(imgdatasize);
	if (!*image) { crash("\nLoadTGA: %s: out of memory!\n",file); __fclose(fp); return 1; }
	
	__fread(*image, imgdatasize, 1, fp);

	//lprintf("ok, %dx%d.\n", header.width, header.height);
	__fclose(fp);
	return 0;	
}
