// editor.c
// Real-time Map Editor module
#include "keen.h"
#include "editor.fdh"
#include "esp_attr.h"

#define MAP_IN_PLAY			0
#define MAP_ORIGINAL		1
EXT_RAM_ATTR unsigned int map_stored_play[MAP_MAXWIDTH][MAP_MAXHEIGHT];
EXT_RAM_ATTR unsigned int map_stored_org[MAP_MAXWIDTH][MAP_MAXHEIGHT];
int whichmap = MAP_ORIGINAL;

int mx, my, lastmx, lastmy;
int mousx, mousy;
char mouse_inside_playfield;
char objects_dirty = 0;

extern int mouse_x, mouse_y;
extern uchar mouseL, mouseR;
int lastmouseL=1, lastmouseR=1;

#define TILES		0
#define ENEMY		1
#define AUTOBLOCK	2
#define NUM_REDRAWS	3
char redraw[NUM_REDRAWS];

int toptile = 0;
int curtile;
int curenemy;
char rungame = 0; char rsflash; int rsflashtimer;
#define RS_FLASH_RATE		300
int blockx1, blocky1, blockstep, blocksavetile;
int switchx1, switchy1;

#define MODE_NORM			0			// normal plot
#define MODE_RESIZEMAP		1			// resizing map
#define MODE_BLOCKFILL		2			// waiting for 2nd coord of block-fill
#define MODE_PUTSWITCH		3			// setting switch ext-plat coords
#define MODE_WAITBUTTONUP	255			// no plotting allowed till button up
uchar plotmode;

int resize_orgw, resize_orgh;

// for editor_entertext/editor_gettext
#define MAXTEXT			19
char entering_text;
char entertext_buffer[MAXTEXT];
int entertext_index;
char entertext_numericonly;
const char *entertextprompt;
void (*entertext_callback)(char *text);

// saves the state objects were in, while the game is paused
EXT_RAM_ATTR stObject saved_objects[MAX_OBJECTS];

char lastsavename[128];

// position where "current enemy" is shown
#define SPR_LEFT			(WINDOW_WIDTH+4)
#define SPR_TOP				20
#define SPRAREA_W			32
#define SPRAREA_H			40
#define NEXTBUTTON_X		(SPR_LEFT-1)
#define NEXTBUTTON_Y		(SPR_TOP+SPRAREA_H+2)
#define LASTBUTTON_X		NEXTBUTTON_X
#define LASTBUTTON_Y		(NEXTBUTTON_Y+BUTTON_H+2)

// positioning of the tile palette
#define TSPACING					2
#define TTOP						(TSPACING+WINDOW_HEIGHT+1)
#define TCOLS						19
#define TROWS						4
#define TILESSHOWN					(TCOLS*TROWS)
#define CURTILE_LEFT				(WINDOW_WIDTH+2)
#define CURTILE_TOP					(WINDOW_HEIGHT-8)
#define COORDS_LEFT					CURTILE_LEFT
#define COORDS_TOP					(CURTILE_TOP-8)

// baddies = objects to show for each baddie type
// badovrlay = sprites to overlay on top of the objects (like arrows showing direction)
int baddies[3][LAST_OBJ_TYPE+1] = {{OBJ_YORP_DEFSPRITE,OBJ_GARG_DEFSPRITE,OBJ_VORT_DEFSPRITE_EP1,OBJ_BUTLER_DEFSPRITE,OBJ_TANK_DEFSPRITE,OBJ_ICECHUNK_DEFSPRITE,OBJ_ICECHUNK_DEFSPRITE,OBJ_ICECHUNK_DEFSPRITE,OBJ_ICECHUNK_DEFSPRITE,OBJ_ROPE_DEFSPRITE,0},\
								   {OBJ_VORT_DEFSPRITE_EP2,OBJ_BABY_DEFSPRITE_EP2,OBJ_VORTELITE_DEFSPRITE,OBJ_WALKER_DEFSPRITE,OBJ_TANKEP2_DEFSPRITE,OBJ_PLATFORM_DEFSPRITE_EP2,OBJ_SPARK_DEFSPRITE_EP2,0},\
								   {OBJ_VORT_DEFSPRITE_EP3,OBJ_BABY_DEFSPRITE_EP3,OBJ_MOTHER_DEFSPRITE,OBJ_MEEP_DEFSPRITE,OBJ_NINJA_DEFSPRITE,OBJ_FOOB_DEFSPRITE,OBJ_BALL_DEFSPRITE,OBJ_JACK_DEFSPRITE,OBJ_PLATFORM_DEFSPRITE_EP3,OBJ_PLATFORM_DEFSPRITE_EP3,\
										OBJ_VORT_JUMP_DEFSPRITE_EP3,-1,-1,ENEMYRAYEP3,RAY_VERT_EP3,0}};
int badovrlay[3][LAST_OBJ_TYPE+1]= {{0,0,0,0,0,ARROWUR_SPRITE,ARROWU_SPRITE,ARROWD_SPRITE,ARROWUL_SPRITE,0,0},\
								   {0,0,0,0,0,0,0},\
								   {0,0,0,0,0,0,0,0,ARROWLR_SPRITE,ARROWUD_SPRITE,0,-1,-1,0,0,0}};

int curblock;

#include "buttons.c"
#include "ghosting.c"
#include "autoblock.c"

								   
void static drawline_h(int y, int x1, int x2, uchar c) { for(;x1<x2;x1++) setpixel(x1,y,c); }
void static drawline_v(int x, int y1, int y2, uchar c) { for(;y1<y2;y1++) setpixel(x,y1,c); }
void static line_h(int y, int x1, int x2, uchar c)
{
	if (options[OPT_ZOOM])
	{
		x1*=2; x2*=2; y*=2;
		drawline_h(y+1, x1, x2, c);
	}
	drawline_h(y, x1, x2, c);
}
void static line_v(int x, int y1, int y2, uchar c)
{
	if (options[OPT_ZOOM])
	{
		x*=2; y1*=2; y2*=2;
		drawline_v(x+1, y1, y2, c);
	}
	drawline_v(x, y1, y2, c);
}
void box(int x, int y, int w, int h, uchar c)
{
	line_h(y, x, x+w, c);
	line_h(y+h, x, x+w, c);
	line_v(x, y, y+h, c);
	line_v(x+w, y, y+h+1, c);
}

char editor_init(void)
{
	numplayers = 1;
	Menu_SetVisibility(0);
	
	///if (!levelcontrol.custom_level_name[0]) strcpy(levelcontrol.custom_level_name, "custom/autolight.ck2");///temphack
	editor_loadmap(levelcontrol.custom_level_name);
	
	setrunstop(0);
	editor_resetmap();
	
	editor_set_do_ghosting(options[OPT_GHOSTING]);
	
	// border around the playfield
	line_h(WINDOW_HEIGHT, 0, WINDOW_WIDTH+1,13);
	line_v(WINDOW_WIDTH, 0, WINDOW_HEIGHT,13);
	
	// draw the next/last buttons
	drawbutton("NEXT", NEXTBUTTON_X, NEXTBUTTON_Y);
	drawbutton("LAST", LASTBUTTON_X, LASTBUTTON_Y);
	drawbutton("NEXT", AB_NEXTBUTTON_X, AB_NEXTBUTTON_Y);
	drawbutton("LAST", AB_LASTBUTTON_X, AB_LASTBUTTON_Y);
	
	curtile = 177;
	curenemy = 0;
	curblock = 0;
	toptile = (curtile / TCOLS) * TCOLS;
	plotmode = MODE_NORM;
	al_cantundo();
	
	entering_text = 0;
	lastmx = lastmy = -1;
	memset(redraw, 1, NUM_REDRAWS);
	
	lprintf("Map editor initialized.\n");
	return 0;
}

int getenemysprite(int e)
{
	return baddies[levelcontrol.episode-1][e];
}

void static drawtiles(void)
{
int x, y;
int t,i,l;
char textbuf1[8], textbuf2[8];

	if (curtile < toptile)
	{
		toptile = (curtile / TCOLS) * TCOLS;
	}
	else if (curtile >= toptile+TILESSHOWN)
	{
		toptile = (curtile / TCOLS) * TCOLS;
		toptile -= (TCOLS*(TROWS-1));
	}
	if (curtile < 0) curtile = 0;
	if (toptile < 0) toptile = 0;
	if (curtile >= MAX_TILES) curtile = MAX_TILES-1;
	if (toptile >= MAX_TILES) toptile = MAX_TILES-1;
	
	y = TTOP;
	t = toptile;
	for(l=0;l<TROWS;l++)
	{
		x = TSPACING;
		for(i=0;i<TCOLS;i++)
		{
			if (t < MAX_TILES) drawtile(x, y, t); else drawtile(x,y,0);
			if (t==curtile)
			{
				box(x, y, 15, 15, 14);
				box(x+1,y+1, 13, 13,0);
			}
			x += (16 + TSPACING);
			t++;
		}
		y += (16 + TSPACING);
	}
	
	sprintf(textbuf2, "%d", curtile);
	for(i=0;i<3-strlen(textbuf2);i++) { textbuf1[i] = ' '; } textbuf1[i] = 0;
	strcat(textbuf1, textbuf2);
	font_draw(textbuf1, CURTILE_LEFT, CURTILE_TOP, drawcharacter_clear_erasebg);
}

void static drawcurenemy(void)
{
char text[40];
int s,x,y,xoff,yoff;

	xoff = SPR_LEFT<<options[OPT_ZOOM];
	yoff = SPR_TOP<<options[OPT_ZOOM];
	s = getenemysprite(curenemy);
	
	// clear the sprite area
	for(y=0;y<SPRAREA_H<<options[OPT_ZOOM];y++)
		for(x=0;x<SPRAREA_W<<options[OPT_ZOOM];x++)
		  setpixel(x+xoff,y+yoff,0);
	
	// draw "cur enemy" sprite
	drawsprite_noclip(xoff, yoff, s);
	
	// draw "overlay" on sprite if needed
	s = badovrlay[levelcontrol.episode-1][curenemy];
	if (s)
	{
		drawsprite_noclip(xoff+4, yoff+4, s);
	}
	
	// show # of current enemy
	sprintf(text,"%d  ",curenemy+1);
	font_draw(text,LASTBUTTON_X+2,LASTBUTTON_Y+BUTTON_H+6,drawcharacter_clear_erasebg);
}

int editor_getnewcurtile(int mx, int my)
{
	if (my < TTOP) return -1;
	if (mx > TSPACING+((16+TSPACING)*TCOLS)) return -1;
	
	my -= TTOP; mx -= TSPACING;
	mx /= (16 + TSPACING);
	my /= (16 + TSPACING);
	
	if (my >= TROWS)
	{
		toptile += TCOLS;
		curtile += TCOLS;
		redraw[TILES] = 1;
		return -1;
	}
	return (my * TCOLS) + mx + toptile;
}

void editor_scrolling(void)
{
int i;
	// scroll 2x faster if LSHIFT down
	for(i=0;i<=KeyDrv_KeyIsDown(SDLK_TAB);i++)
	{
		if (keytable[KRIGHT])
		{
			map_scroll_right();
		}
		else if (keytable[KLEFT])
		{
			map_scroll_left();
		}
		if (keytable[KDOWN])
		{
			map_scroll_down();
		}
		else if (keytable[KUP])
		{
			map_scroll_up();
		}
	}
}

void editor_clearmap(void)
{
int x, y;

	// DO NOT change to map.xsize/map.ysize--it's important to clear whole
	// map in case they resize it bigger later
	for(y=0;y<MAP_MAXHEIGHT;y++)
	for(x=0;x<MAP_MAXWIDTH;x++)
	{
		map.mapdata[x][y] = 143;
		map.objectlayer[x][y] = 0;
	}
	map_coat_border();
	storemap(MAP_ORIGINAL);
	storemap(MAP_IN_PLAY);
	
	lastsavename[0] = 0;
	map_setstartpos(4, 4);
	SpecialRegion_SetDefaults();
	editor_resetmap();
}

void editor_resetmap(void)
{
int m;
	al_cantundo();
	editor_resetobjects();
	game_SetStartPositions();
	m = whichmap;
	setmap(MAP_ORIGINAL); storemap(MAP_IN_PLAY);
	whichmap = m;
	take_all_keycards();
	gamedo_resettimelimit();
	initplayers();
	player[0].inventory.HasPogo = (levelcontrol.episode!=1)?1:0;
	player[0].inventory.score = 0;
	player[0].pdir = player[0].pshowdir = (player[0].x>>CSF>>TILE_S < map.xsize/2 || levelcontrol.episode==1)?RIGHT:LEFT;
	switch(levelcontrol.episode)
	{
		case 1: player[0].inventory.charges = 0; break;
		case 2: player[0].inventory.charges = 3; break;
		case 3: player[0].inventory.charges = 5; break;
	}
	gamepdo_SelectFrame(0);
	map_calc_max_scroll();
	scroll_to_player();
	plotmode = MODE_NORM;
	makedark(0);
	ClearConsoleMsgs();
}

void editor_addborder(void)
{
int x,y;
	// add the visible border found on all original ep1 levels
	if (levelcontrol.episode != 3)
	{
		for(y=3;y<map.ysize-2;y++)
		{
			plotmap(2, y, 258);
			plotmap(map.xsize-3, y, 258);
		}
		for(x=3;x<map.xsize-3;x++)
		{
			plotmap(x, 2, 275);
		}
		plotmap(2, 2, 273);
		plotmap(map.xsize - 3, 2, 274);
		if (levelcontrol.episode==2)
		{
			Console_Msg("Warning: ep2 usually has no border");
		}
	}
	else
		Console_Msg("You can't add a border in episode 3.");
}

void make_enemies_visible(void)
{
int i;
	for(i=highest_objslot-1;i>=1;i--)
	{
		if (objects[i].exists && objects[i].type!=OBJ_PLAYER)
			gamedo_calcenemyvisibility(i);
	}
}

void editor_keyshortcuts(void)
{
int x,y;

	if (mousx < WINDOW_WIDTH && mousy < WINDOW_HEIGHT)
	{
		if (plotmode==MODE_NORM)
		{
			// object deletion
			if (!rungame && KeyDrv_KeyIsDown(SDLK_DELETE) && !KeyDrv_LastKeyIsDown(SDLK_DELETE))
			{
				for(x=0;x<=1;x++)
				 for(y=0;y<=1;y++)
					if (map.objectlayer[mx-x][my-y])
					{
						map.objectlayer[mx-x][my-y] = 0;
						break;
					}
				editor_resetobjects();
			}
			
			if (KeyDrv_KeyIsDown(SDLK_q) && !KeyDrv_LastKeyIsDown(SDLK_q))
			{
				map_setstartpos(mx, my);
				player[0].pinertia_x = player[0].pinertia_y = 0;
				reviveplayer(0);
				sound_stop(SOUND_KEEN_DIE);
			}
			
			if (KeyDrv_KeyIsDown(SDLK_LSHIFT) && !KeyDrv_LastKeyIsDown(SDLK_LSHIFT))
			{
				if (curtile != map.mapdata[mx][my])
				{
					curtile = map.mapdata[mx][my];
					redraw[TILES] = 1;
				}
			}
		
		}
		
		if (KeyDrv_KeyIsDown(SDLK_b) && !KeyDrv_LastKeyIsDown(SDLK_b))
		{
			if (plotmode == MODE_BLOCKFILL)
			{
				plotmap(blockx1, blocky1, blocksavetile);
				Console_Msg("Blockfill canceled");
				plotmode = MODE_WAITBUTTONUP;
			}
			else
			{
				blockx1 = mx; blocky1 = my;
				blocksavetile = map.mapdata[mx][my];
				plotmap(mx, my, curtile);
				plotmode = MODE_BLOCKFILL;
			}
		}
		
		if (KeyDrv_KeyIsDown(SDLK_l) && !KeyDrv_LastKeyIsDown(SDLK_l))
		{
			autolight(mx, my);
		}
	
	}
	
	if (KeyDrv_KeyIsDown(SDLK_m) && !KeyDrv_LastKeyIsDown(SDLK_m))
	{
		editor_resizemap();
	}
	
	if (KeyDrv_KeyIsDown(SDLK_z) && !KeyDrv_LastKeyIsDown(SDLK_z))
	{
		me_togglerunstop(1);
	}
	// reset positions
	if (KeyDrv_KeyIsDown(SDLK_x) && !KeyDrv_LastKeyIsDown(SDLK_x))
	{
		editor_resetmap();
	}
	
	if (KeyDrv_KeyIsDown(SDLK_w) && !KeyDrv_LastKeyIsDown(SDLK_w))
	{
		curtile -= TCOLS;
		redraw[TILES] = 1;
	}
	if (KeyDrv_KeyIsDown(SDLK_s) && !KeyDrv_LastKeyIsDown(SDLK_s))
	{
		curtile += TCOLS;
		redraw[TILES] = 1;
	}
	if (KeyDrv_KeyIsDown(SDLK_a) && !KeyDrv_LastKeyIsDown(SDLK_a))
	{
		curtile--;
		redraw[TILES] = 1;
	}
	if (KeyDrv_KeyIsDown(SDLK_d) && !KeyDrv_LastKeyIsDown(SDLK_d))
	{
		curtile++;
		redraw[TILES] = 1;
	}
	
	if (KeyDrv_KeyIsDown(SDLK_e) && !KeyDrv_LastKeyIsDown(SDLK_e))
	{
		editor_nextep();
	}
	
	if (KeyDrv_KeyIsDown(SDLK_COMMA) && !KeyDrv_LastKeyIsDown(SDLK_COMMA))
	{
		chgcurenemy(-1);
	}
	else if (KeyDrv_KeyIsDown(SDLK_PERIOD) && !KeyDrv_LastKeyIsDown(SDLK_PERIOD))
	{
		chgcurenemy(1);
	}
	if (KeyDrv_KeyIsDown(SDLK_SEMICOLON) && !KeyDrv_LastKeyIsDown(SDLK_SEMICOLON))
	{
		chgcurautoblock(-1);
	}
	else if (KeyDrv_KeyIsDown(SDLK_QUOTE) && !KeyDrv_LastKeyIsDown(SDLK_QUOTE))
	{
		chgcurautoblock(1);
	}
	
	if (!rungame)
	{
		if (KeyDrv_KeyIsDown(SDLK_g) && !KeyDrv_LastKeyIsDown(SDLK_g))
		{
			editor_set_do_ghosting(options[OPT_GHOSTING] ^ 1);
		}
		
		if (keytable[KF1] && !last_keytable[KF1])
		{
			editor_save();
		}
		
		if (keytable[KF2] && !last_keytable[KF2])
		{
			editor_saveas();
		}
	}
}

void static editor_saveas_callback(char *fname)
{
	if (fname[0])
	{
		sprintf(lastsavename, "%s%s.ck%c", CUSTOM_DIR, fname, levelcontrol.episode+'0');
		editor_save();
	}
	else editor_saveas();
}
void editor_saveas(void)
{
	editor_entertext("Save As: ", 0, editor_saveas_callback);
}
void editor_save(void)
{
	if (lastsavename[0])
	{
		if (rungame) setrunstop(0);
		ClearConsoleMsgs();
		entering_text = 0;
		savemap(lastsavename);
	}
	else editor_saveas();
}

int tl_mins, tl_secs;
void editor_settimelimit(void)
{
	editor_entertext("Mins (or ENTER to remove):", 1, timelimit_getsecs_cb);
}
void static timelimit_getsecs_cb(char *buf)
{
	if (buf[0]==0)
	{
		timelimit_remove();
	}
	else
	{
		tl_mins = atoi(buf);
		if (tl_mins >= 0 && tl_mins <= 99)
		{
			editor_entertext("Secs (or ENTER):", 1, timelimit_set_cb);
		}
		else
		{
			Console_Msg("Invalid value: must be 0-99.");
		}
	}
}
void static timelimit_set_cb(char *buf)
{
	if (buf[0]==0)
	{
		timelimit_remove();
	}
	else
	{
		tl_secs = atoi(buf);
		if (tl_secs >= 0 && tl_secs <= 59)
		{
			timelimit_set(tl_mins, tl_secs);
		}
		else
		{
			Console_Msg("Invalid value: must be 0-59.");
		}
	}
}
void static timelimit_remove(void)
{
	Console_Msg("Level time limit removed.");
	map.hastimelimit = 0;
	gamedo_resettimelimit();
}
void static timelimit_set(int mins, int secs)
{
	Console_Msg("Time limit set: %02d:%02d", mins, secs);
	map.hastimelimit = 1;
	map.time_m = mins;
	map.time_s = secs;
	gamedo_resettimelimit();
}

// pops up a prompt "prompt" and allows the user to enter text.
// when enter is pushed, the function "callback" will be called
// with a pointer to the text.
int flashtimer, flashcur;
void editor_entertext(const char *prompt, char numericonly, void (*callback)(char *text))
{
	if (rungame) setrunstop(0);
	entering_text = 1;
	entertext_index = 0;
	entertextprompt = prompt;
	entertext_callback = callback;
	entertext_numericonly = numericonly;
	flashtimer = 0; flashcur = 1;
}

// the text handler which implements editor_entertext.
void static editor_gettext(void)
{
char chars[40] = {SDLK_SPACE,' ',SDLK_UNDERSCORE,'_',SDLK_MINUS,'_',SDLK_0,'0',SDLK_1,'1',SDLK_2,'2',SDLK_3,'3',SDLK_4,'4',SDLK_5,'5',SDLK_6,'6',SDLK_7,'7',SDLK_8,'8',SDLK_9,'9',0,0};
int i;
	if (++flashtimer > 40) { flashtimer = 0; flashcur ^= 1; }
	entertext_buffer[entertext_index] = 0;
	Console_Msg("%s%s%c", entertextprompt, entertext_buffer, flashcur?'_':' ');
	
	if (entertext_index < MAXTEXT && (strlen(entertextprompt) + entertext_index) < (WINDOW_WIDTH/8)-4)
	{
		if (!entertext_numericonly)
		{
			for(i=SDLK_a;i<=SDLK_z;i++)
			{
				if (KeyDrv_KeyIsDown(i) && !KeyDrv_LastKeyIsDown(i))
				{
					entertext_buffer[entertext_index++] = (i - SDLK_a) + 'A';
					return;
				}
			}
		}
		for(i=0;chars[i];i+=2)
		{
			if (!entertext_numericonly || (chars[i+1] >= '0' && chars[i+1] <= '9'))
			{
				if (KeyDrv_KeyIsDown(chars[i]) && !KeyDrv_LastKeyIsDown(chars[i]))
				{
					entertext_buffer[entertext_index++] = chars[i+1];
					return;
				}
			}
		}
	}
	
	if (KeyDrv_KeyIsDown(SDLK_BACKSPACE) && !KeyDrv_LastKeyIsDown(SDLK_BACKSPACE))
	{
		if (entertext_index > 0) entertext_index--;
		return;
	}
	if (KeyDrv_KeyIsDown(SDLK_RETURN) && !KeyDrv_LastKeyIsDown(SDLK_RETURN))
	{
		ClearConsoleMsgs();
		entering_text = 0;
		if (entertext_callback) (*entertext_callback)(entertext_buffer);
		return;
	}
	if (KeyDrv_KeyIsDown(SDLK_ESCAPE) && !KeyDrv_LastKeyIsDown(SDLK_ESCAPE))
	{
		ClearConsoleMsgs();
		entering_text = 0;
		keytable[KESC] = 0;		// don't pop open menu
	}
}


void swap(int *a, int *b)
{
int aa, bb;
	aa = *a; bb = *b;
	*a = bb; *b = aa;
}

// this function is called when the user clicks on the playfield with the
// left mouse button (also called continuously as long as the button is held).
void static clicked_playfield(int x, int y)
{
	al_cantundo();
	
	// normal plot of map tile
	if (plotmode==MODE_NORM)
	{
		plotmap(mx,my,curtile);
		
		// if we just plotted a switch, find out where they want
		// the platform
		if (levelcontrol.episode==2 && (curtile==TILE_SWITCH_UP||curtile==TILE_SWITCH_DOWN))
		{
			switchx1 = mx; switchy1 = my;
			plotmode = MODE_PUTSWITCH;
		}
		return;
	}
	
	// all other modes require the mouse button to be clicked, not held down
	if (lastmouseL) return;
	
	
	switch(plotmode)
	{
		case MODE_RESIZEMAP:
			editor_setmapsize(mx+3, my+3);
			map_coat_border();
			Console_Msg("New map size: [%dx%d]", map.xsize, map.ysize);
		break;
		
		case MODE_BLOCKFILL:
			{
				int blockx2, blocky2, step;
				blockx2 = mx; blocky2 = my;
				if (blockx1 > blockx2) swap(&blockx1, &blockx2);
				if (blocky1 > blocky2) swap(&blocky1, &blocky2);
				
				if (KeyDrv_KeyIsDown(SDLK_LSHIFT))
				{
					autoblock(blockx1, blocky1, blockx2, blocky2);
				}
				else
				{
					step = KeyDrv_KeyIsDown(SDLK_LCTRL) ? 2:1;
					for(y=blocky1;y<=blocky2;y+=step)
					for(x=blockx1;x<=blockx2;x+=step)
					{
						plotmap(x,y,curtile);
					}
				}
				Console_Msg("Blockfill from [%d,%d]-[%d,%d]",blockx1,blocky1,blockx2,blocky2);
			}
		break;
		
		case MODE_PUTSWITCH:
			{
				uint spos; uchar sx, sy;
				// get relative distance between the platform & switch
				sx = (mx - switchx1);
				sy = (my - switchy1);
				spos = (sy << 8) | sx;
				map.objectlayer[switchx1][switchy1] = spos;
				if (spos)
				{
					Console_Msg("Extending platform set to [%d,%d]: %04x",mx,my,spos);
				}
				else
				{
					Console_Msg("Switch will activate Tantalus Ray.");
				}
			}
		break;
		
		
		default:
			crash("unknown 'plotmode': %d\n", plotmode);
			break;
	}
	
	// don't normal plot again until button released and they click again
	plotmode = MODE_WAITBUTTONUP;

}


void editor_run(void)
{
int i;
char txt[80];
char *ptr;

	mousx = (mouse_x>>options[OPT_ZOOM]);
	mousy = (mouse_y>>options[OPT_ZOOM]);
	mx = (scroll_x + mousx) >> TILE_S;
	my = (scroll_y + mousy) >> TILE_S;
	mouse_inside_playfield = (mousx < WINDOW_WIDTH && mousy < WINDOW_HEIGHT);
	
	
	if (mx != lastmx || my != lastmy)
	{
		if (mouse_inside_playfield)
		{
			sprintf(txt, "%02x%02x", mx, my);
			font_draw(txt, COORDS_LEFT, COORDS_TOP, drawcharacter_clear_erasebg);
		}
		lastmx = mx; lastmy = my;
	}
	
	switch(plotmode)
	{
		case MODE_RESIZEMAP:
			Console_Msg("Map resize: click on lower-left corner");
			break;
		
		case MODE_PUTSWITCH:
			Console_Msg("Click to spec pos of switch's platform");
			break;
			
		case MODE_BLOCKFILL:
			if (KeyDrv_KeyIsDown(SDLK_LALT)) ptr = "Autoblock";
			else if (KeyDrv_KeyIsDown(SDLK_LCTRL)) ptr = "Blockfill x2";
			else ptr = "Blockfill";
			Console_Msg("%s - click mouse at 2nd coord", ptr);
			break;
	}
		
		
	// left mouse button handler
	if (mouseL)
	{
		if (mouse_inside_playfield)
		{
			clicked_playfield(mx, my);
		}
		else if (!lastmouseL)
		{
			// check for click on any buttons
			if (intersectbutton(NEXTBUTTON_X, NEXTBUTTON_Y, mousx, mousy))
			{
				chgcurenemy(1);
			}
			else if (intersectbutton(LASTBUTTON_X, LASTBUTTON_Y, mousx, mousy))
			{
				chgcurenemy(-1);
			}
			else if (intersectbutton(AB_NEXTBUTTON_X, AB_NEXTBUTTON_Y, mousx, mousy))
			{
				chgcurautoblock(1);
			}
			else if (intersectbutton(AB_LASTBUTTON_X, AB_LASTBUTTON_Y, mousx, mousy))
			{
				chgcurautoblock(-1);
			}
			else
			{
				// check for a click on the tile palette
				int c = editor_getnewcurtile(mousx, mousy);
				if (c != -1) { curtile = c; redraw[TILES] = 1; }
			}
		}
	}
	else	// mouse button NOT down
	{
		if (plotmode==MODE_WAITBUTTONUP) plotmode = MODE_NORM;
	}
	
	
	// right mouse button handler
	if (!rungame && mouseR && !lastmouseR && mouse_inside_playfield)
	{
		map.objectlayer[mx][my] = curenemy+1;
		editor_resetobjects();
	}
	
	// keyboard handler
	if (!Menu_IsVisible())
	{
		if (!entering_text)
			editor_keyshortcuts();
		else
			editor_gettext();
	}
	else
	{
		plotmode = MODE_WAITBUTTONUP;
		ClearConsoleMsgs();
	}
	
			
	if (redraw[TILES])
	{
		drawtiles();
		redraw[TILES] = 0;
	}
	if (redraw[ENEMY])
	{
		drawcurenemy();
		redraw[ENEMY] = 0;
	}
	if (redraw[AUTOBLOCK])
	{
		drawcurautoblock();
		redraw[AUTOBLOCK] = 0;
	}
	
	if (!rungame)
	{
		if (!Menu_IsVisible()) { editor_scrolling(); }
		gamedo_getInput();
		gamedo_HandleInput();
		Menu_HandleMenu();
		gamedo_fades();
		gamedo_AnimatedTiles();
		gamedo_togglemenus();
		make_enemies_visible();
		memcpy(last_keytable, keytable, KEYTABLE_SIZE);
		overlay.statusbox = 0;
	}
	else
	{
		// stop run if player gets killed
		if (player[0].pdie && !player[0].pdietillfly)
		{
			setrunstop(0);
		}
		else
		{
			// run the game
			gamedo_gamelogic();
			
			// flash the "RUN" message
			if (++rsflashtimer > RS_FLASH_RATE)
			{
				rsflashtimer = 0; rsflash ^= 1;
				showrunstop();
			}
		}
	}
	
	lastmouseL = mouseL; lastmouseR = mouseR;
	KeyDrv_CopyLastKeys();
}


void storemap(int whichmap)
{
int x,y;
	for(y=0;y<map.ysize;y++)
	 for(x=0;x<map.xsize;x++)
	 {
		if (whichmap==MAP_IN_PLAY)
			map_stored_play[x][y] = map.mapdata[x][y];
		else
			map_stored_org[x][y] = map.mapdata[x][y];
	 }
	 whichmap = MAP_ORIGINAL;
}

void setmap(int new)
{
int x,y;
	if (new != whichmap)
	{
		storemap(whichmap);
		whichmap = new;
		
		for(y=0;y<map.ysize;y++)
		 for(x=0;x<map.xsize;x++)
		 {
			if (whichmap==MAP_IN_PLAY)
				map_chgtile(x, y, map_stored_play[x][y]);
			else
				map_chgtile(x, y, map_stored_org[x][y]);
		 }
	}
}

void plotmap(int x, int y, int t)
{
	if (map.mapdata[x][y] != t)
	{
		map_stored_org[x][y] = t;
		map_stored_play[x][y] = t;
		map_chgtile(x, y, t);
		// erase object information for ep2 extending-platform switches
		if (map.objectlayer[x][y] > LAST_OBJ_TYPE)
		{
			map.objectlayer[x][y] = 0;
		}
	}
}

void showrunstop(void)
{
int x,y,xa,ya,c;
	#define RS_LEFT	(WINDOW_WIDTH+4)
	#define RS_TOP	(4)
	#define RS_RIGHT	(RS_LEFT+(4*8))
	#define RS_BTM		(RS_TOP+(1*8))
	if (rungame)
	{
		c = rsflash?12:0;
		for(y=RS_TOP;y<RS_BTM;y++)
		for(x=RS_LEFT;x<RS_RIGHT;x++)
		{
			if (options[OPT_ZOOM])
			{
				xa = x*2; ya = y*2;
				setpixel(xa, ya, c);
				setpixel(xa, ya+1, c);
				setpixel(xa+1, ya, c);
				setpixel(xa+1, ya+1, c);
			}
			else setpixel(x, y, c);
		}
		font_draw("RUN", RS_LEFT+4, RS_TOP, drawcharacter_clear);
	}
	else
	{
		font_draw("STOP", RS_LEFT, RS_TOP, drawcharacter_clear_erasebg);
	}
}

void setrunstop(int new)
{
int i;
	plotmode = MODE_NORM;
	al_cantundo();
	
	lprintf("> editor: setrunstop(): %d\n", new);
	rungame = new;
	if (rungame)
	{
		if (map.hastimelimit && levelcontrol.time_left_m==0 && levelcontrol.time_left_s==0) gamedo_resettimelimit();
		restoreobjectstates();
		reviveplayer(0);
		scroll_to_player();
		setmap(MAP_IN_PLAY);
		TimeDrv_ResetSecondsTimer();
		for(i=1;i<highest_objslot;i++)
		{
			if (objects[i].exists && objects[i].type != OBJ_PLAYER)
				objects[i].hasbeenonscreen = 0;
		}
		GiveAnkh(0);
		player[0].ankhtime = 450;
		sound_stop(SOUND_ANKH);
		player[0].inventory.lives = 4;
		
		rsflash = 1; rsflashtimer = 0;
	}
	else
	{
		storeobjectstates();
		resetobjects();
		if (options[OPT_GHOSTING]) spawn_ghosted_objects();
		objects_dirty = 1; redraw[ENEMY] = 1;
		
		setmap(MAP_ORIGINAL);
		sound_stop_all();
	}
	showrunstop();
}

// store object states because we're about to go to "stop" mode
void storeobjectstates(void)
{
	memcpy(saved_objects, objects, sizeof(objects));
}
// restore object states to the way they were the game was stopped
void restoreobjectstates(void)
{
	memcpy(objects, saved_objects, sizeof(objects));
	objects_dirty = 0; redraw[ENEMY] = 1;
}

// changes curenemy to the next or last enemy in the list.
// pass positive value when "Next" button pushed, negative value when "Last"
// button pushed.
void static chgcurenemy(int incamt)
{
	do
	{
		if (incamt > 0)
		{
			curenemy++;
			if (!getenemysprite(curenemy)) curenemy = 0;
		}
		else
		{
			if (!curenemy)
			{
				while(getenemysprite(curenemy)) curenemy++;
			}
			curenemy--;
		}
	} while (getenemysprite(curenemy)==-1);
	
	redraw[ENEMY] = 1;
}

void static chgcurautoblock(int incamt)
{
int *block;
	if (incamt > 0)
	{
		curblock++;
		block = getautoblock(curblock);
		if (block[0]==-1) curblock = 0;
	}
	else
	{
		if (curblock > 0)
			curblock--;
		else
		{
			curblock = 0;
			do
			{
				block = getautoblock(curblock);
				if (block[0]==-1)
				{
					curblock--;
					break;
				}
				curblock++;
			} while(1);
		}
	}
	redraw[AUTOBLOCK] = 1;
}

char editor_gameisrunning(void)
{
	return rungame;
}

// changes episode to "ep"
void editor_chgep(int ep)
{
	al_cantundo();
	// fix the "last saved" filename, so that if they save, it saves
	// under the correct extension
	if (lastsavename[0])
	{
		lastsavename[strlen(lastsavename)-1] = '0' + ep;
	}
	
	if (Load_Episode(ep))
	{
		crash("editor_chgep: failed loading episode %d\n", ep);
	}
	editor_resetobjects();
	editor_resetmap();
	map_coat_border(); storemap(MAP_IN_PLAY); storemap(MAP_ORIGINAL);
	map_redraw();
	curenemy = 0;
	player[0].inventory.HasPogo = (levelcontrol.episode == 1)?0:1;
	memset(redraw, 1, NUM_REDRAWS);
	curblock = 0;
	if (rungame) setrunstop(0);
}

void editor_nextep(void)
{
int ep;
	sound_play(SOUND_GUN_CLICK, PLAY_NOW);
	
	ep = levelcontrol.episode;
	do
	{
		ep++; if (ep > 3) ep = 1;
	} while(!episode_available[ep]);
	
	editor_chgep(ep);
}

// enter Editor mode
void editor_enter(void)
{
	endlevel(ENTERING_EDITOR);
	Menu_SetVisibility(0);
}

// exit Editor mode
void editor_exit(void)
{
	endlevel(LEAVING_EDITOR);
}

void static editor_resetobjects(void)
{
	resetobjects();
	storeobjectstates();
	objects_dirty = 0; redraw[ENEMY] = 1;
}

void editor_loadmap(char *fname)
{
char *ext;
uchar ep;

	lprintf("> editor_loadmap: '%s'\n", fname);
	al_cantundo();
	
	// if fname is "", create a new map
	if (!fname[0])
	{
		lprintf("editor_loadmap: creating new map\n");
		StartGame(levelcontrol.episode, "", 1);
		map.xsize = map.ysize = 128;
		editor_clearmap();
	}
	else if ((ext = strrchr(fname, '.')))
	{
		if (toupper(ext[1])=='C' && toupper(ext[2])=='K')
		{
			if (ext[3] >= '1' && ext[3] <= '3')
			{
				ep = ext[3] - '0';
				
				// important for in case they resize the map bigger
				editor_clearmap();
				
				editor_chgep(ep);
				StartGame(ep, "", 1);
				strcpy(lastsavename, fname);
				if (loadmap(lastsavename))
				{
					crash("editor_loadmap: failed loading map %s", lastsavename);
				}
			}
		}
	}
	else
	{
		crash("editor_loadmap: '%s' is not a valid filename.", fname);
	}
	
	levelcontrol.curlevel = 1;
	storemap(MAP_ORIGINAL);
	storemap(MAP_IN_PLAY);
	whichmap = MAP_ORIGINAL;
	initgame();
	map_redraw();
}

void editor_setmapsize(int xs, int ys)
{
	if (xs <= MAP_MINWIDTH) xs = MAP_MINWIDTH+1;
	if (ys <= MAP_MINHEIGHT) ys = MAP_MINHEIGHT+1;
	if (xs > MAP_MAXWIDTH) xs = MAP_MAXWIDTH;
	if (ys > MAP_MAXHEIGHT) ys = MAP_MAXHEIGHT;
	
	map.xsize = xs; map.ysize = ys;
	map_calc_max_scroll();
	while(scroll_x > max_scroll_x) map_scroll_left();
	while(scroll_y > max_scroll_y) map_scroll_up();
}

void editor_resizemap(void)
{
	if (plotmode==MODE_RESIZEMAP)
	{
		Console_Msg("Map resize: canceled");
		editor_setmapsize(resize_orgw, resize_orgh);
		plotmode = MODE_WAITBUTTONUP;
	}
	else
	{
		resize_orgw = map.xsize; resize_orgh = map.ysize;
		editor_setmapsize(MAP_MAXWIDTH, MAP_MAXHEIGHT);
		plotmode = MODE_RESIZEMAP;
	}
}

void editor_wonlevel(void)
{
	setrunstop(0);
	editor_resetmap();
}
