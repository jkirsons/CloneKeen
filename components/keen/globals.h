// see globals.c
#ifndef MAXPATHLEN
	#define MAXPATHLEN	1024
#endif

extern int whodied;
extern int newgame_episode;
extern int newgame_number_of_players;
extern char custom_path[MAXPATHLEN];
extern int highest_objslot;
extern int editor;
extern int last_editor;

extern uchar background[SCROLLBUF_XSIZE][SCROLLBUF_YSIZE];

extern char episode_available[4];

extern char fading_to_leave_gameloop;

extern char window_is_fullscreen;

extern char PlatExtending;
extern stFadeControl fadecontrol;
extern stMap map;
extern unsigned int AnimTileInUse[ATILEINUSE_SIZEX][ATILEINUSE_SIZEY];
extern stTile tiles[MAX_TILES];
extern char localmp;
extern unsigned char tiledata[MAX_TILES][16][16];
extern stSprite sprites[MAX_SPRITES];
extern stBitmap bitmaps[MAX_BITMAPS];
extern stAnimTile animtiles[MAX_ANIMTILES];
extern char font[MAX_FONT][8][8];
extern char font_clear[MAX_FONT][8][8];
extern stObject objects[MAX_OBJECTS];
extern stPlayer player[MAX_PLAYERS];
extern stPlayer net_lastplayer[MAX_PLAYERS];
extern int options[NUM_OPTIONS];
extern unsigned char *scrollbuf;
extern unsigned char *blitbuf;
extern char keytable[KEYTABLE_SIZE];
extern char keytable[KEYTABLE_SIZE];
extern char last_keytable[KEYTABLE_SIZE];
extern int opx,opy;
extern int font_start;
extern char QuitState;

extern stString strings[MAX_STRINGS];
extern int numStrings;

extern int animtiletimer, curanimtileframe;

extern uchar primaryplayer;
extern uchar numplayers;

extern char frameskiptimer;

extern unsigned long scroll_x;
extern unsigned int scrollx_buf;
extern unsigned char scrollpix;
extern unsigned int mapx;
extern unsigned int mapxstripepos;
extern unsigned long scroll_y;
extern unsigned int scrolly_buf;
extern unsigned char scrollpixy;
extern unsigned int mapy;
extern unsigned int mapystripepos;

extern char crashflag;

extern unsigned int objdefsprites[LAST_OBJ_TYPE+1];

extern int max_scroll_x, max_scroll_y;
extern int atilescount,aobjectscount;
extern int playerbaseframes[MAX_PLAYERS];

extern stLevelControl levelcontrol;
extern stOverlays overlay;

extern char loadslot;

extern char *BitmapData;

extern char ScreenIsScrolling;
extern int blockedby;
extern int gunfiretimer, gunfirefreq;
extern char cheatmode;

extern int NessieObjectHandle;
extern EgaHead LatchHeader;

extern int framebyframe, framestorun;

extern stDemo demo;

extern char otherplayer;

extern char crash_reason[2048];
extern char caitlin_isnt_awesome;
