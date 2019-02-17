/* FILEIO.C
  Functions responsible for loading/saving data from files.
*/

#include "keen.h"
#include "fileio.fdh"

// open a keen datafile.
// if a custom episode is being played, automatically looks for
// a version inside the custom episode directory first.
FILE *fileopen(char *name, char *mode)
{
FILE *fp;

	if (mode[0]=='r')
	{
		if (levelcontrol.custom_episode_path[0])
		{
			uchar customname[MAXPATHLEN];
			// try loading from the custom ep path first
			strcpy(customname, levelcontrol.custom_episode_path);
			strcat(customname, name);
			
			fp = fcaseopen(customname, mode);
			if (fp)
			{
				lprintf("using custom-episode version of: '%s'\n", customname);
				return fp;
			}
		}
	}
	
	return fcaseopen(name, mode);
}

// opens a file case-insensitively, as it was on DOS
FILE *fcaseopen(char *ci_name, char *mode)
{
FILE *fp;

	// first try it with original capitilization
	fp = __fopen(ci_name, mode);
	if (fp) return fp;
	
	if (mode[0]=='r')
	{
		int prefix, ext;
		char fname[MAXPATHLEN];
		int fn_length = strlen(ci_name);
		int i;
		
		// try various permutations to see if we can get it open
		strcpy(fname, ci_name);
		
		for(ext=1;ext>=0;ext--)
		{
			for(prefix=1;prefix>=0;prefix--)
			{
				char hitdot = 0;
				
				for(i=fn_length-1;i>=0;i--)
				{
					if (fname[i]=='.') { hitdot = 1; continue; }
					if (fname[i]=='/' || fname[i]=='\\') break;
					
					if (hitdot)
						fname[i] = prefix ? toupper(fname[i]) : tolower(fname[i]);
					else
						fname[i] = ext ? toupper(fname[i]) : tolower(fname[i]);
				}
				
				// try that and see if we can open it
				__stat("trying '%s'", fname);
				fp = __fopen(fname, mode);
				if (fp) return fp;
			}
		}
	}
	
	return NULL;
}

/*
void c------------------------------() {}
*/

unsigned int fgeti(FILE *fp) {
unsigned int lsb, msb;
  lsb = __fgetc(fp);
  msb = __fgetc(fp);
  return (msb<<8) | lsb;
}

void fputi(uint word, FILE *fp)
{
	__fputc(word&255, fp);
	__fputc(word/256, fp);
}

unsigned long fgetl(FILE *fp) {
unsigned int temp1, temp2, temp3, temp4;
  temp1 = __fgetc(fp);
  temp2 = __fgetc(fp);
  temp3 = __fgetc(fp);
  temp4 = __fgetc(fp);
  return (temp4<<24) | (temp3<<16) | (temp2<<8) | temp1;
}

void fputl(ulong word, FILE *fp)
{
unsigned long a,b,c,d;
	a=b=c=d = word;
	a &= 0xFF000000; a >>= 24;
	b &= 0x00FF0000; b >>= 16;
	c &= 0x0000FF00; c >>= 8;
	d &= 0x000000FF;
	__fputc(d, fp);
	__fputc(c, fp);
	__fputc(b, fp);
	__fputc(a, fp);
}


// load a 32-bit RGBA TGA file into sprite 's', and add colors to the palette
// as needed so that it can be shown exactly as found in the file.
// returns nonzero on failure.
char LoadTGASprite(char *filename, int s)
{
uchar *image, *base;
int x,y;
int w,h;
uchar r,g,b,a;
int c;
uchar fname[MAXPATHLEN];

	sprintf(fname, "%s%s", GFXDIR, filename);
	if (LoadTGA(fname, &image, &w, &h))
	{
		lprintf("LoadTGASprite: LoadTGA failed opening %s\n", fname);
		return 1;
	}
	
	if (w > MAX_SPRITE_WIDTH || h > MAX_SPRITE_HEIGHT)
	{
		free(image);
		lprintf("LoadTGASprite: image %s too big (%dx%d, max %dx%d)!\n", fname, w, h, MAX_SPRITE_WIDTH, MAX_SPRITE_HEIGHT);
		return 1;
	}
	
	lprintf("LoadTGASprite: Parse %s; %dx%d\n", fname, w, h);
	sprites[s].imgdata = alloc2D(h, w);
  sprites[s].maskdata = alloc2D(h, w);
	base = image;
	for(y=h-1;y>=0;y--)
	for(x=0;x<w;x++)
	{
		b = *image++; g = *image++; r = *image++; a = *image++;
		if (a & 128)
		{		
			c = pal_getcolor(r, g, b);
			if (c==-1) c = pal_addcolor(r, g, b);
			if (c==-1)
			{
				crash("LoadTGASprite: out of colorspace");
				return 1;
			}
			
			sprites[s].imgdata[y][x] = c;
			sprites[s].maskdata[y][x] = 1;
		}
		else
		{
			sprites[s].maskdata[y][x] = 0;
		}
	}
	sprites[s].xsize = w;
	sprites[s].ysize = h;
	
	// detect the sprite's bounding box
	for(x=0;x<sprites[s].xsize-1;x++)
	{
		for(y=0;y<sprites[s].ysize;y++) if (sprites[s].maskdata[y][x]) goto break1;
	}
break1: ;
	sprites[s].bboxX1 = x << CSF;
	
	for(x=sprites[s].xsize-1;x>0;x--)
	{
		for(y=0;y<sprites[s].ysize;y++) if (sprites[s].maskdata[y][x]) goto break2;
	}
break2: ;
	sprites[s].bboxX2 = x << CSF;
	
	for(y=0;y<sprites[s].ysize-1;y++)
	{
		for(x=0;x<sprites[s].xsize-1;x++) if (sprites[s].maskdata[y][x]) goto break3;
	}
break3: ;
	sprites[s].bboxY1 = y << CSF;
	
	for(y=sprites[s].ysize-1;y>0;y--)
	{
		for(x=0;x<sprites[s].xsize-1;x++) if (sprites[s].maskdata[y][x]) goto break4;
	}
break4: ;
	sprites[s].bboxY2 = y << CSF;
	
	//lprintf("> sprite %d bounding box: (%d,%d)-(%d,%d)\n",s,sprites[s].bboxX1,sprites[s].bboxY1,sprites[s].bboxX2,sprites[s].bboxY2);
	
	free(base);
	return 0;
}


char loadtileattributes(int episode)
{
FILE *fp;
int t,a,b,c,intendedep,intendedver;
char fname[80];

  sprintf(fname, "ep%dattr.dat", episode);

//  printf("loading tile attributes from '%s'...\n", fname);

  fp = fileopen(fname, "rb");
  if (!fp)
  {
	crash("loadtileattributes(): Cannot open tile attribute file %s!\n", fname);
    return 1;
  }

  /* check the header */
  // header format: 'A', 'T', 'R', episode, version
  a = __fgetc(fp);
  b = __fgetc(fp);
  c = __fgetc(fp);
  if (a != 'A' || b != 'T' || c != 'R')
  {
     crash("loadtileattributes(): Attribute file corrupt! ('ATR' marker not found)");
     return 1;
  }

  intendedep = __fgetc(fp);
  if (intendedep != episode)
  {
     crash("loadtileattributes(): file is intended for episode %d, but you're trying to use it with episode %d!\n", intendedep, episode);
     return 1;
  }

  intendedver = __fgetc(fp);
  if (intendedver < ATTRFILEVERSION)
  {
	crash("(attr file version %d, I need version %d)\n", intendedver, ATTRFILEVERSION);
	crash("loadtileattributes(): %s is too old for this version of CloneKeen!\n", fname);
	return 1;
  }
  else if (intendedver > ATTRFILEVERSION)
  {
     crash("(file version %d, I need version %d)\n", intendedver, ATTRFILEVERSION);
     crash("loadtileattributes(): %s is too new for this version of CloneKeen!\n", fname);
     return 1;
  }

  /* load in the tile attributes */

  for(t=0;t<MAX_TILES-1;t++)
  {
  
    tiles[t].solidl = __fgetc(fp);
    if (tiles[t].solidl==-1)
    {
      crash("loadtileattributes(): %s corrupt! (unexpected EOF)\n", fname);
      return 1;
    }

    tiles[t].solidr = __fgetc(fp);
    tiles[t].solidfall = __fgetc(fp);
    tiles[t].solidceil = __fgetc(fp);
    tiles[t].ice = __fgetc(fp);
    tiles[t].semiice = __fgetc(fp);
    tiles[t].priority = __fgetc(fp);
    if (__fgetc(fp)) tiles[t].masktile=t+1; else tiles[t].masktile = 0;
    tiles[t].goodie = __fgetc(fp);
    tiles[t].standgoodie = __fgetc(fp);
    tiles[t].pickupable = __fgetc(fp);
	tiles[t].points = fgeti(fp);
    tiles[t].lethal = __fgetc(fp);
    tiles[t].bonklethal = __fgetc(fp);
    tiles[t].chgtile = fgeti(fp);
    tiles[t].isAnimated = __fgetc(fp);
    tiles[t].animOffset = __fgetc(fp);
    tiles[t].animlength = __fgetc(fp);

  }
  __fclose(fp);

  return 0;
}

char loadstrings_AddAttr(char *attr, int stringIndex)
{
char stAttrName[80];
char stAttrValue[80];
int attrvalue;
int RAMAllocSize;
char *copyPtr;
int i;

  // if the attribute does not have an equals sign bail
  if (!strstr(attr, "="))
  {
    lprintf("loadstrings_AddAttr(): '%s' is not a valid attribute definition.\n", attr);
    return 1;
  }

  // split the attribute up into it's name and it's value
  copyPtr = stAttrName;
  for(i=0;i<strlen(attr);i++)
  {
    if (attr[i] != ' ' && attr[i]!=9)      // strip out spaces and tabs
    {
      if (attr[i] != '=')
      {
        *copyPtr = attr[i];
        copyPtr++;
      }
      else
      { // hit the equals sign
        *copyPtr = 0;
        copyPtr = stAttrValue;
      }
    }
  }
  *copyPtr = 0;

  attrvalue = atoi(stAttrValue);

  // malloc space for the attribute name
  RAMAllocSize = strlen(stAttrName) + 1;
  strings[stringIndex].attrnames[strings[stringIndex].numAttributes] = malloc(RAMAllocSize+1);
  if (!strings[stringIndex].attrnames[strings[stringIndex].numAttributes])
  {
    lprintf("loadstrings_AddAttr(): Unable to allocate space for attribute name ('%s').\n", stAttrName);
    return 1;
  }

  // copy the data into the strings structure
  memcpy(strings[stringIndex].attrnames[strings[stringIndex].numAttributes], stAttrName, RAMAllocSize);
  strings[stringIndex].attrvalues[strings[stringIndex].numAttributes] = attrvalue;

  strings[stringIndex].numAttributes++;
  return 0;
}

// loads strings from file "strings.dat"
char strings_loaded = 0;
char loadstrings(void)
{
FILE *fp;
char state;
unsigned char stName[80];
unsigned char stString[1024];
unsigned char stAttr[80];
int i,c;
int nameIndex, stringIndex, attrIndex;
int waitChar, gotoState;
char highlight;
int RAMSize;
char *RAMPtr;

  #define STSTATE_WAITCHAR      0
  #define STSTATE_READNAME      1
  #define STSTATE_READSTRING    2
  #define STSTATE_READATTR      3

  if (strings_loaded) { freestrings(); }
  
  lprintf("loadstrings(): Opening strings.dat.\n");
  fp = fileopen("strings.dat", "rb");
  if (!fp)
  {
    lprintf("loadstrings(): Unable to open strings.dat.\n");
    return 1;
  }

  // go through all the strings and NULL out the entries...this will
  // let us know which ones are in use (and need to be free()d at shutdown)
  for(i=0;i<MAX_STRINGS;i++)
  {
    strings[i].name = NULL;
    strings[i].stringptr = NULL;
    strings[i].numAttributes = 0;
  }

  nameIndex = 0;
  stringIndex = 0;
  numStrings = 0;
  highlight = 0;

  // read until we get to the first string name
  state = STSTATE_WAITCHAR;
  waitChar = '[';
  gotoState = STSTATE_READNAME;
  do
  {
    c = __fgetc(fp);              // read byte from file

    if (c<0)
    {   // EOF
      break;
    }
    // ignore LF's
    if (c==10) continue;

    switch(state)
    {
     case STSTATE_WAITCHAR:
      // ignore chars until we read a waitChar, then go to state gotoState
      if (c==waitChar)
      {
        state = gotoState;
      }
      break;
     case STSTATE_READATTR:
      if (c==13)
      { // reached CR, start reading string
        if (attrIndex)
        {
          stAttr[attrIndex] = 0;
          if (loadstrings_AddAttr(stAttr, numStrings)) return 1;
        }
        state = STSTATE_READSTRING;
      }
      else if (c==' ')
      { // end of an attribute definition
        if (attrIndex)
        {
          stAttr[attrIndex] = 0;  // null-terminate
          if (loadstrings_AddAttr(stAttr, numStrings)) return 1;
        }
        attrIndex = 0;
      }
      else
      { // save char to attribute buffer
        stAttr[attrIndex] = c;
        attrIndex++;
      }
      break;
     case STSTATE_READNAME:
      // read in the string name until we get to ']'
      if (c != ']')
      {
        stName[nameIndex] = c;
        nameIndex++;
      }
      else
      {
        stName[nameIndex] = 0;  //null-terminate
	highlight = 0;
        // read any attributes until the CR
        state = STSTATE_READATTR;
        attrIndex = 0;
      }
      break;
     case STSTATE_READSTRING:
      // read in string data until we see another '['
      if (c != '[')
      {
        // allow delimiters:
        // you can put [ and ] in the string by using \( and \).
        // set a highlight (change font color to the +128 font) with \H
        // stop highlighting with \h
        if (stringIndex>0 && stString[stringIndex-1]=='\\'+(highlight*128))
        {  // delimiter detected
          if (c=='(')
          {
            stString[stringIndex - 1] = '[' + (highlight*128);
          }
          else if (c==')')
          {
            stString[stringIndex - 1] = ']' + (highlight*128);
          }
          else if (c=='H')
          {
            highlight = 1;
            stringIndex--;
          }
          else if (c=='h')
          {
            highlight = 0;
            stringIndex--;
          }
          else if (c=='\\')
          {
            stString[stringIndex - 1] = '\\' + (highlight*128);
          }
        }
        else
        { // normal non-delimited char
          stString[stringIndex] = c;
          if (highlight && c!=0 && c!=13)
          {
            stString[stringIndex] += 128;
          }
          stringIndex++;
        }
      }
      else
      {
        stString[stringIndex-1] = 0;  //null-terminate (cutting off final CR)

        /* save the string to the strings[] structure */

        // we're going to malloc() an area and copy the name, then the string,
        // into it. We'll need room for both the name and the string, plus
        // null-terminators for each.
        RAMSize = strlen(stName) + strlen(stString) + 2;
	RAMPtr = malloc(RAMSize);
        if (!RAMPtr)
        {
          lprintf("loadstrings(): Could not allocate memory for string '%s'\n", stName);
          return 1;
        }

        // assign our pointers
        strings[numStrings].name = &RAMPtr[0];
        strings[numStrings].stringptr = &RAMPtr[strlen(stName)+1];

        // copy the string info to the newly malloc()'d memory area
        memcpy(strings[numStrings].name, stName, strlen(stName)+1);
        memcpy(strings[numStrings].stringptr, stString, strlen(stString)+1);

        numStrings++;
        // read the name of the next string
        state = STSTATE_READNAME;
        nameIndex = 0;
        stringIndex = 0;
      }
      break;
    }

  } while(1);

  lprintf("loadstrings(): loaded %d strings.\n", numStrings);
  strings_loaded = 1;
  __fclose(fp);
  return 0;
}

int freestrings(void)
{
int i,j;
int NumStringsFreed;

  NumStringsFreed = 0;
  for(i=0;i<MAX_STRINGS;i++)
  {
    if (strings[i].name)
    {
      // free the string name
      free(strings[i].name);
      strings[i].name = strings[i].stringptr = NULL;
      // free all attribute names
      for(j=0;j<strings[i].numAttributes;j++)
      {
        free(strings[i].attrnames[j]);
      }
      strings[i].numAttributes = 0;

      NumStringsFreed++;
    }
  }

  strings_loaded = 0;
  return NumStringsFreed;
}

char *MissingString = "MISSING STRING!";

// returns a pointer to the string with name 'name'
char* getstring(char *name)
{
int i;
  for(i=0;i<numStrings;i++)
  {
    if (!strcmp(name, strings[i].name))
    {
      return strings[i].stringptr;
    }
  }

  return MissingString;
}

// returns attribute attrname of string stringname, or -1 if it doesn't exist.
int GetStringAttribute(char *stringName, char *attrName)
{
int i,j;

  for(i=0;i<numStrings;i++)
  {
    if (!strcmp(stringName, strings[i].name))
    {
      // we found the string, now find the requested attribute
      for(j=0;j<strings[i].numAttributes;j++)
      {
        if (!strcmp(attrName, strings[i].attrnames[j]))
        {
          return strings[i].attrvalues[j];
        }
      }
      // failed to find attribute
      return -1;
    }
  }
  // failed to find string
  return -1;
}



void DefaultOptions(void)
{
	lprintf("Setting default options. (First time run?)\n");
	memset(options, 0, NUM_OPTIONS);
	options[OPT_RENDERRATE] = 60;
	options[OPT_ZOOM] = 0;
	options[OPT_ZOOMONRESTART] = 0;
	options[OPT_2BUTTONFIRING] = 1;
	options[OPT_RISEBONUS] = 1;
	options[OPT_SHOWOSD] = 1;
	options[OPT_SHOWCARDS] = 1;
	options[OPT_DOORSBLOCKRAY] = 1;
	options[OPT_P1COLOR] = PCOLOR_STD;
	options[OPT_P2COLOR] = PCOLOR_RED;
	options[OPT_GHOSTING] = 1;
}

void LoadOptions(void)
{
int i;
int ver, num;
char key[40];	

	ver = Ini_GetNumericKey(NULL, "OptVer");
	num = Ini_GetNumericKey(NULL, "OptNum");
	
	// check for wrong ini file version (or no ini file at all!)
	if (ver != CFGFILEVERSION || num != NUM_OPTIONS)
	{
		lprintf("Incorrect config file version detected. Will reset it.\n");
		DefaultOptions();
		Ini_WriteKey(NULL, "CAITLIN", "AWESOME");
		return;
	}
	
	lprintf("Loading config file.\n");
	for(i=0;i<NUM_OPTIONS;i++)
	{
		sprintf(key, "OPT%d", i);
		options[i] = Ini_GetNumericKey(NULL, key);
	}
	
	Ini_GetKey(NULL, "CAITLIN", key);
	caitlin_isnt_awesome = (strcmp(key, "AWESOME") ? 1:0);
  options[OPT_ZOOM] = 0;
  options[OPT_ZOOMONRESTART] = 0;
}

void SaveOptions(void)
{
int i;
char key[40];	
	
	lprintf("Saving config file.\n");
	
	Ini_WriteNumericKey(NULL, "OptVer", CFGFILEVERSION);
	Ini_WriteNumericKey(NULL, "OptNum", NUM_OPTIONS);
	
	for(i=0;i<NUM_OPTIONS;i++)
	{
		sprintf(key, "OPT%d", i);
		Ini_WriteNumericKey(NULL, key, options[i]);
	}
}

void DumpOptions(void)
{
int i;
	lprintf("Options [%d, %d]:\n ", CFGFILEVERSION, NUM_OPTIONS);
	for(i=0;i<NUM_OPTIONS;i++)
		lprintf("%d%s ", options[i], (i+1==NUM_OPTIONS)?"":",");
		
	lprintf("\n");
}
