#ifndef TGA_H
#define TGA_H

#define TGA_NONE		0
#define TGA_INDEXED		1
#define TGA_RGB			2
#define TGA_GREYSCALE	3

#include "SDL_system.h"
#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long

/* located in fileio.c */

//---------------------[referenced from fileio.c]--------------------//
FILE *fileopen(char *name, char *mode);
FILE *fcaseopen(char *ci_name, char *mode);
unsigned int fgeti(FILE *fp);
void fputi(uint word, FILE *fp);
unsigned long fgetl(FILE *fp);
void fputl(ulong word, FILE *fp);
char LoadTGASprite(char *filename, int s);
char loadtileattributes(int episode);
char loadstrings_AddAttr(char *attr, int stringIndex);
char loadstrings(void);
int freestrings(void);
char* getstring(char *name);
int GetStringAttribute(char *stringName, char *attrName);
void DefaultOptions(void);
void LoadOptions(void);
void SaveOptions(void);
void DumpOptions(void);

/* located in lprintf.c */

//---------------------[referenced from lprintf.c]-------------------//
void lprintf(const char *str, ...);
void crash(const char *str, ...);
char lprintf_init(void);
void lprintf_close(void);

typedef struct
{
    uchar identsize;           // size of ID field that follows 18 uchar header (usually 0)
    uchar colourmaptype;       // type of colour map 0=none, 1=has palette
    uchar imagetype;           // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

    uint colourmapstart;       // first colour map entry in palette
    uint colourmaplength;      // number of colours in palette
    uchar colourmapbits;       // number of bits per palette entry 15,16,24,32

    uint xstart;               // image x origin
    uint ystart;               // image y origin
    uint width;                // image width in pixels
    uint height;               // image height in pixels
    uchar bpp;	               // image bits per pixel 8,16,24,32
    uchar descriptor;          // image descriptor bits (vh flip bits)
    
    // pixel data follows header    
} TGA_HEADER;

#endif
