// routines for handling save&load of savegames

#include "keen.h"
#include "savegame.fdh"

#define SG_HEADERSIZE			8


char game_save(int slot)
{
FILE *fp;
char fname[20];
int i;

	// can't save game under certain circumstances
	if (fade_in_progress()) return 1;
	
	
	sprintf(fname, "savegame%d.dat", slot);
		
	lprintf("Saving game to %s\n", fname);
	fp = fileopen(fname, "wb");
	if (!fp) return 1;
	
	fprintf(fp, "CKSAVE%c", SAVEGAMEVERSION);
	__fputc(IsBigEndian(), fp);
	__fputc(levelcontrol.episode, fp);
	__fputc(levelcontrol.curlevel, fp);
	__fputc(player[0].inventory.lives, fp);
	__fputc(numplayers, fp);
	__fputc(levelcontrol.play_custom_level, fp);
	__fputc(levelcontrol.custom_episode_path[0]?1:0, fp);
	__fputc(primaryplayer, fp);
	
	sgrle_compress(fp, (unsigned char *)&levelcontrol, sizeof(levelcontrol));
	
	sgrle_compress(fp, (unsigned char *)&scroll_x, sizeof(scroll_x));
	sgrle_compress(fp, (unsigned char *)&scrollx_buf, sizeof(scrollx_buf));
	sgrle_compress(fp, (unsigned char *)&scrollpix, sizeof(scrollpix));
	sgrle_compress(fp, (unsigned char *)&mapx, sizeof(mapx));
	sgrle_compress(fp, (unsigned char *)&mapxstripepos, sizeof(mapxstripepos));
	
	sgrle_compress(fp, (unsigned char *)&scroll_y, sizeof(scroll_y));
	sgrle_compress(fp, (unsigned char *)&scrolly_buf, sizeof(scrolly_buf));
	sgrle_compress(fp, (unsigned char *)&scrollpixy, sizeof(scrollpixy));
	sgrle_compress(fp, (unsigned char *)&mapy, sizeof(mapy));
	sgrle_compress(fp, (unsigned char *)&mapystripepos, sizeof(mapystripepos));
	
	sgrle_compress(fp, (unsigned char *)&max_scroll_x, sizeof(max_scroll_x));
	sgrle_compress(fp, (unsigned char *)&max_scroll_y, sizeof(max_scroll_y));
	
	sgrle_compress(fp, (unsigned char *)&map, sizeof(map));
	
	fputi(highest_objslot, fp);
	sgrle_compress(fp, (unsigned char *)&objects[0], sizeof(objects));
	sgrle_compress(fp, (unsigned char *)&tiles[0], sizeof(tiles));
	sgrle_compress(fp, (unsigned char *)&fadecontrol, sizeof(fadecontrol));	
	
	for(i=0;i<numplayers;i++)
	{
		sgrle_compress(fp, (unsigned char *)&player[i], sizeof(player[i]));
	}
	
	for(i=0;i<NUM_OPTIONS;i++)
	{
		if (i != OPT_FULLSCREEN && i != OPT_ZOOM && i != OPT_ZOOMONRESTART)
			__fputc(options[i], fp);
	}
	
	// save state of partially-opened doors
	__fputc(sprites[DOOR_YELLOW_SPRITE].ysize, fp);
	__fputc(sprites[DOOR_RED_SPRITE].ysize, fp);
	__fputc(sprites[DOOR_GREEN_SPRITE].ysize, fp);
	__fputc(sprites[DOOR_BLUE_SPRITE].ysize, fp);
	
	__fclose(fp);
	return 0;
}


char IsValidSaveGame(char *fname)
{
FILE *fp;
int i;
char *verify = "CKSAVE";
	fp = fileopen(fname, "rb");
	if (!fp) return 0;
	
	for(i=0;i<strlen(verify);i++)
	{
		if (__fgetc(fp) != verify[i])
		{
			__fclose(fp);
			return 0;
		}
	}
	if (__fgetc(fp) != SAVEGAMEVERSION)
	{
		__fclose(fp);
		return 0;
	}
	if (__fgetc(fp) != IsBigEndian())
	{
		__fclose(fp);
		return 0;
	}
	__fclose(fp);
	return 1;
}

// this is seperated out of game_load for modularity because menumanager.c
// also uses it, in it's save-game "preview" menu on the load game screen
void savegame_readheader(FILE *fp, uchar *episode, uchar *level, uchar *lives, uchar *num_players, uchar *is_custom)
{
char custom = 0;
	__fseek(fp, SG_HEADERSIZE, SEEK_SET);		// skip past the CKSAVE%c
	*episode = __fgetc(fp);
	*level = __fgetc(fp);
	*lives = __fgetc(fp);
	*num_players = __fgetc(fp);
	
	custom = __fgetc(fp);
	custom |= __fgetc(fp);
	*is_custom = custom;
}

char game_load(int slot)
{
FILE *fp;
char fname[20];
uchar episode, level, lives, iscustom;
int i;

	
	sprintf(fname, "savegame%d.dat", slot);
	
	if (!IsValidSaveGame(fname))
	{
		lprintf("%s is not a valid save-game.\n", fname);
		return 1;
	}
	
	lprintf("Loading game from file %s\n", fname);
	fp = fileopen(fname, "rb");
	if (!fp) { lprintf("unable to open %s\n",fname); return 1; }
	
	savegame_readheader(fp, &episode, &level, &lives, &numplayers, &iscustom);
	// note that we don't have to load the LEVEL, because the state
	// of the map is already saved inside the save-game.
	
	lprintf("game_load: restoring structures...\n");
	primaryplayer = __fgetc(fp);
	
	sgrle_initdecompression();
	if (sgrle_decompress(fp, (unsigned char *)&levelcontrol, sizeof(levelcontrol))) return 1;
	if (Load_Episode(episode))
	{
		crash("game_load: failed loading episode %d\n", episode);
		return 1;
	}
	
	if (sgrle_decompress(fp, (unsigned char *)&scroll_x, sizeof(scroll_x))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&scrollx_buf, sizeof(scrollx_buf))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&scrollpix, sizeof(scrollpix))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&mapx, sizeof(mapx))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&mapxstripepos, sizeof(mapxstripepos))) return 1;
	
	if (sgrle_decompress(fp, (unsigned char *)&scroll_y, sizeof(scroll_y))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&scrolly_buf, sizeof(scrolly_buf))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&scrollpixy, sizeof(scrollpixy))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&mapy, sizeof(mapy))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&mapystripepos, sizeof(mapystripepos))) return 1;
	
	if (sgrle_decompress(fp, (unsigned char *)&max_scroll_x, sizeof(max_scroll_x))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&max_scroll_y, sizeof(max_scroll_y))) return 1;
	
	if (sgrle_decompress(fp, (unsigned char *)&map, sizeof(map))) return 1;
	
	highest_objslot = fgeti(fp);
	if (sgrle_decompress(fp, (unsigned char *)&objects[0], sizeof(objects))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&tiles[0], sizeof(tiles))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&fadecontrol, sizeof(fadecontrol))) return 1;
	
	for(i=0;i<numplayers;i++)
	{
		if (sgrle_decompress(fp, (unsigned char *)&player[i], sizeof(player[i]))) return 1;
	}
	
	for(i=0;i<NUM_OPTIONS;i++)
	{
		if (i != OPT_FULLSCREEN && i != OPT_ZOOM && i != OPT_ZOOMONRESTART)
			options[i] = __fgetc(fp);
	}
	
	sprites[DOOR_YELLOW_SPRITE].ysize = __fgetc(fp);
	sprites[DOOR_RED_SPRITE].ysize = __fgetc(fp);
	sprites[DOOR_GREEN_SPRITE].ysize = __fgetc(fp);
	sprites[DOOR_BLUE_SPRITE].ysize = __fgetc(fp);
	
	__fclose(fp);
	lprintf("Structures restored: map size: %d,%d\n", map.xsize, map.ysize);
	lprintf("Redrawing map...\n");
	map_redraw();
	lprintf("Load game OK\n");
	pal_setdark(levelcontrol.dark);
	memset(&overlay, 0, sizeof(overlay));
	return 0;
}


void DeleteSaveGame(int slot)
{
char fname[20];

	sprintf(fname, "savegame%d.dat", slot);
	remove(fname);	
}
