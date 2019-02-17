// basic ini file parser
// ripped from a beta version of another one of my games and stripped down

#include <stdio.h>
#include "keen.h"			// for fileopen() define on platforms that don't use it
#include "ini.fdh"

// name of the config file for keen, and the default if NULL is passed as
// the ini filename to a read/write function.
char *configfile = "keen.conf";


// buffered lines out of ini file for speed of lookups
#define MAXLINES			500
char *linebuf[MAXLINES];
unsigned int nIniLines = 0;

unsigned int Ini_IsBuffered = 0;
char Ini_BufferedName[MAXPATHLEN];
char Ini_CustomPath[MAXPATHLEN];

char static BufferIni(char *filename)
// load the given INI file into the line buffer so it can parsed quickly
{
FILE *fp;
unsigned char line[512];

	Ini_FreeBuffer();
	strcpy(Ini_BufferedName, filename);
	strcpy(Ini_CustomPath, levelcontrol.custom_episode_path);
	
	fp = fileopen(filename, "rb");
	if (!fp)		// file doesn't exist
	{
		nIniLines = 0;
		Ini_IsBuffered = 1;
		return 0;
	}
   
	// load in each line and skip comments etc
	Ini_IsBuffered = 0;
	nIniLines = 0;
	while(!__feof(fp))
	{
		line[0] = 0;
		fgets(line, sizeof(line)-1, fp);
		// trim the CR that fgets returns
		while(line[strlen(line)-1]==13||line[strlen(line)-1]==10) line[strlen(line)-1]=0;
		// skip comments and blank lines
		if (line[0]==0 || line[0]=='#') continue;
		
		// malloc() space for the line and it's terminator and copy it in
		if ((linebuf[nIniLines]=(char *)malloc(strlen(line) + 1))==NULL)
		{
			lprintf("BufferIni: Out of Memory!\n");
			Ini_FreeBuffer();
			return 1;
		}
		strcpy(linebuf[nIniLines], line);
		nIniLines++;
		
		if (nIniLines > MAXLINES)
		{
			lprintf("BufferIni: too many lines in file %s (maximum %d)\n", filename, MAXLINES);
			return 1;
		}
	}
	__fclose(fp);
	
	lprintf("BufferIni: %d lines buffered out of %s.\n", nIniLines, filename);
	Ini_IsBuffered = 1;
	return 0;
}

char BufferIfNeeded(char *fname)
{
	// if game.ini is not already buffered, buffer it now
	if (!Ini_IsBuffered || strcmp(Ini_BufferedName, fname) || strcmp(Ini_CustomPath, levelcontrol.custom_episode_path))
	{
		return BufferIni(fname);
	}
	return 0;
}

// save the INI file to disk, from the buffer
void static saveini(void)
{
FILE *fp;
int i;
	if (!Ini_IsBuffered) return;
	
	fp = fileopen(Ini_BufferedName, "wb");
	if (!fp) { lprintf("saveini: unable to open %s\n", Ini_BufferedName); return; }
	
	for(i=0;i<nIniLines;i++)
	{
		fprintf(fp, "%s\r\n", linebuf[i]);
	}
	__fclose(fp);
}


char Ini_FreeBuffer(void)
// flushes out all the buffered .ini lines
{
int i;
	if (Ini_IsBuffered)
	{
		for(i=0;i<nIniLines;i++)
			free(linebuf[i]);
		
		Ini_IsBuffered = 0;
		Ini_BufferedName[0] = 0;
		Ini_CustomPath[0] = 0;
		return 1;
	}
	return 0;
}

// retrieves the information at the given *key copies the data into *result.
// returns zero if succesful or nonzero on nonexistant key, or error.
// inifile - the name of the .ini file to read. if NULL, the standard
// config file is used.
char Ini_GetKey(char *inifile, char *key, char *output)
{
char line[256];
char *index;
int curline;

	output[0] = 0;                // if there's an error, we'll return ""
	
	if (inifile)
	{
		if (BufferIfNeeded(inifile)) return 1;
	}
	else
	{
		if (BufferIfNeeded(configfile)) return 1;
	}
   
	curline = 0;
	while(curline < nIniLines)
	{
		strcpy(line, linebuf[curline++]);
		
		// get index of "="
		index = strchr(line, '=');
		if (index)
		{		
			// split string at "="
			*index = 0;
			
			// check if it's the key we're looking for
			if (!strcasecmp(line, key))
			{
				index++;
				strcpy(output, index);
				return 0;
			}
		}
	}
	
	// EOF reached: the key doesn't exist
	return 1;
}

// changes the value of the given *key to *newval, or if the key doesn't
// exist, it will create it. Returns nonzero on error, like if there are
// too many keys in the file.
// inifile - the name of the .ini file to write to. if NULL, the standard
// config file is used.
char Ini_WriteKey(char *inifile, char *key, char *newval)
{
char line[256];
char newline[256];
char *index;
int curline;

	if (inifile)
	{
		if (BufferIfNeeded(inifile)) return 1;
	}
	else
	{
		if (BufferIfNeeded(configfile)) return 1;
	}
	
	// get the new line to write
	sprintf(newline, "%s=%s", key, newval);
	
	// first try to find the line that contains the current value, if there is one
	curline = 0;
	while(curline < nIniLines)
	{
		strcpy(line, linebuf[curline]);
		
		// get index of "="
		index = strchr(line, '=');
		if (index)
		{
			// split string at "="
			*index = 0;
			       
			// check if it's the key we're looking for
			if (!strcasecmp(line, key))
			{
				// update the line with a new line
				break;
			}
		}
		
		curline++;
	}
	
	if (curline >= nIniLines)
	{
		if (curline+1 >= MAXLINES) return 1;		// too many lines in file
		nIniLines = curline+1;
	}
	else
		free(linebuf[curline]);
	
	linebuf[curline] = (char *)malloc(strlen(newline) + 1);
	if (!linebuf[curline]) { crash("Ini_WriteKey: Out of Memory!\n"); return 1; }
	
	strcpy(linebuf[curline], newline);
	saveini();
	
	return 0;
}


// convenience functions
int Ini_GetNumericKey(char *inifile, char *key)
{
char tempbuf[256];
	return !Ini_GetKey(inifile, key, tempbuf) ? atoi(tempbuf) : -1;
}

int Ini_GetNumericKeyDef(char *inifile, char *key, int def)
{
char tempbuf[256];
	return !Ini_GetKey(inifile, key, tempbuf) ? atoi(tempbuf) : def;
}

char Ini_WriteNumericKey(char *inifile, char *key, int value)
{
char tempbuf[256];
	sprintf(tempbuf, "%d", value);
	return Ini_WriteKey(inifile, key, tempbuf);
}

