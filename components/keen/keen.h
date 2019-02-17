#ifndef KEEN_H
#define KEEN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <SDL.h>

#define uchar unsigned char
//#include "game.fdh"



#define REVISION			"CloneKeen version 8.4"

//#define PORTEDBY1	"PORTED TO xxx BY"
//#define PORTEDBY2	"myname suchandsuch"

#ifndef PORTEDBY1
	#ifdef __HAIKU__
		#define PORTEDBY1	"Haiku build"
	#else
		#define PORTEDBY1	""
	#endif
#endif
#ifndef PORTEDBY2
	#define PORTEDBY2	""
#endif

#define SAVEGAMEVERSION		7
#define DEMOVERSION			5
#define CFGFILEVERSION		1
#define ATTRFILEVERSION		2

#define WINDOW_WIDTH		320
#define WINDOW_HEIGHT		240
#define WINDOW_2X_WIDTH		(WINDOW_WIDTH*2)
#define WINDOW_2X_HEIGHT	(WINDOW_HEIGHT*2)

// directory "custom" maps and episode are under
#define CUSTOM_DIR		"custom/"
// directory "special" sprites and VGASprites/VGATiles are under
#define GFXDIR			"gfx/"
// name of temporary file when recording a demo
#define DEMO_TMP		"demo.tmp"

#ifndef MAXPATHLENLEN
#define MAXPATHLENLEN		1024
#endif

#define uchar		unsigned char
#define uint		unsigned short
#define ulong		unsigned long

#ifndef WIN32
	#define CASE_SENSITIVE_FILESYSTEM
#endif

#ifdef __GNUC__
//	#pragma GCC poison	printf
#endif

#include "osparam.h"
#include "sounds.h"
#include "latch.h"
#include "game.h"
#include "keys.h"
#include "macros.h"

// because Makegen can't catch function pointers yet
void drawcharacter_clear(int xoff, int yoff, int f);
void drawcharacter_clear_inverse(int xoff, int yoff, int f);
void drawcharacter_clear_erasebg(int xoff, int yoff, int f);
void drawcharacter(int xoff, int yoff, int f);
void sb_drawcharacter(int xoff, int yoff, int f);

// define the Coord Scale Factor. This is used as a kind of
// "floating point" coordinate system which lets objects be at
// position between two pixel positions.
#define CSF    5

// macro which returns true if the specified key is pressed, but wasn't
// pressed last time around the game loop
#define key_just_pressed(whatkey) (keytable[whatkey] && !last_keytable[whatkey])


// for changing p1 and p2 colors--see gpl_compliance.c
#define PCOLOR_STD			0
#define PCOLOR_YELGREEN		1
#define PCOLOR_BLUEGREEN	2
#define PCOLOR_RED			3
#define PCOLOR_LAST			4

//#define OVSIZE    3000

// when crashflag is activated by setting it to QUIT_NONFATAL,
// the application will immediately shut down, however the
// "a Fatal Error Occurred" message box will not pop up and
// the sysbeep will not sound.
#define QUIT_NONFATAL   555

#define MAX_TILES    700
#define MAX_SPRITES  500
#define MAX_SPRITE_WIDTH	96
#define MAX_SPRITE_HEIGHT	64
#define MAX_FONT     256
#define MAX_BITMAPS  20

#define MAX_OBJECTS			180
#define MAX_ANIMTILES		200

#define PAL_FADE_SHADES   20
#define PAL_FADE_WHITEOUT 40
typedef struct stFadeControl
{
	int mode;
	int dir;
	int curamt;
	int fadetimer;
	int rate;
	char fadeout_complete;
} stFadeControl;
#define NO_FADE         0
#define FADE_GO         1
#define FADE_COMPLETE   2

#define FADE_IN         1
#define FADE_OUT        2

#define FADE_NORM       3
#define FADE_FAST       1
#define FADE_SLOW       30
#define FADE_FLASH		255		//temphack

#define NO_QUIT                 0
#define QUIT_PROGRAM            1
#define QUIT_TO_TITLE           2

#define MAX_LEVELS     	 100
#define SCROLLBUF_XSIZE  512
#define SCROLLBUF_YSIZE  512
#define SCROLLBUF_MEMSIZE ((SCROLLBUF_XSIZE*2)*((SCROLLBUF_YSIZE*2)+1))
#define TILE_W			16
#define TILE_H			16
#define TILE_S			4
#define SCROLLBUF_NUMTILESX (SCROLLBUF_XSIZE / TILE_W)
#define SCROLLBUF_NUMTILESY (SCROLLBUF_YSIZE / TILE_H)
#if SCROLLBUF_XSIZE==512
	#define SCROLLBUF_SHIFTAMT	9
#else
	#if SCROLLBUF_XSIZE==1024
		#define SCROLLBUF_SHIFTAMT	10
	#else
		#error You need to define a new bit-shift value for SCROLLBUF_SHIFTAMT
		#error because you are using an usually-sized scroll buffer.
	#endif
#endif

#define BLITBUF_XSIZE  WINDOW_WIDTH
#define BLITBUF_YSIZE  WINDOW_HEIGHT
#define BLITBUF_MEMSIZE ((BLITBUF_XSIZE)*(BLITBUF_YSIZE+30))

 // for each entry in the animtileinuse array that is nonzero, that
 // location on the display is an animated tile which is currently registered
 // in animtiles[]. Used in map_draw_hstripe and map_draw_vstripe.
 // When drawing a new stripe over one that previously contained an animated
 // tile, this lets it know it needs to unregister the animated tile that
 // used to be there. the nonzero value corresponds to the associated entry
 // in animtiles[]. the x,y pixel position is the index in here * 16.
 #define ATILEINUSE_SIZEX               ((SCROLLBUF_NUMTILESX)+1)
 #define ATILEINUSE_SIZEY               ((SCROLLBUF_NUMTILESY)+1)

#define MAX_PLAYERS            2

#define WORLD_MAP              80
#define FINAL_LEVEL            16

// values returned by gameloop()
#define QUITTING_PROGRAM		0
#define WON_LEVEL				1
#define LOST_LEVEL				2
#define HIT_TANTALUS_SWITCH		3
#define FOUND_SECRET_LEVEL		4
#define STARTING_NEW_GAME		5
#define STARTING_CUSTOM_LEVEL	6
#define STARTING_CUSTOM_EPISODE	7
#define LOADING_GAME			8
#define ENDING_GAME				9
#define RESTARTING_LEVEL		10
#define LEAVING_EDITOR			11
#define ENTERING_EDITOR			12
#define NOREASON				50		// means it crashed
#define ENTERED_LEVEL		100	// return from world map. level # entered is added to this value

 
// values for levelcontrol.gamemode
#define GM_NORMAL	            0
#define GM_DEMORECORD           1
#define GM_DEMOPLAYBACK         2

typedef struct stLevelControl
{
 // level control
 char gameinprogress;		  // if 0 we're just running the demo at main menu
 char gamemode;				  // normal, record demo, or play demo
 // data about current level
 int curlevel;                // number of current level
 char bonuswarp;			  // flag to warp to ep1 bonus level when returning to map
 char canexit;                // 1 if player is allowed to use the exit door
 char gameover;				  // 1 if "Game Over" is displayed
 char dark;                   // 1 if level is currently dark (lights are out)

 int episode;                 // which episode we're playing (1-3)
 
	// for custom levels
	char play_custom_level;
	char custom_level_name[1024];
	// for custom episodes which patch parts of the game (like gfx).
	// if != "", files are always attemped to be opened from this
	// path first, if present.
	char custom_episode_path[1024];
	
	int time_left_m;		// minutes remaining (for timed games)
	uchar time_left_s;		// seconds remaining
	char died_cause_time_over;
	// end custom level crap
	
	
 int numyorps;				  // # of yorps left (for kill-all-the-yorps levels)
 
 // if 1, the screen will not scroll when keen gets near the edge
 char dontscroll;
 
 // for ep2: how many sparks (tantalus ray machines) are left
 // you must destroy the tantalus ray generator before exiting
 int sparks_left;
 
 int gametoload; char loadgame_freeze;

 // if 1, a moving platform is currently extending/retracting (ep2)
 char PlatExtending;
 
 // if > 0, the screen will shake and it will decrement each frame.
 // used when you push a switch on a tantalus ray (ep2), and Mortimer's machine
 int vibratetime;
 // if 1, then while vibrating the game will be paused
 char vibratepause;
 
 // boolean array showing which levels have been completed
 // (have "Done" tiles over them on the world map)
 char levels_completed[MAX_LEVELS+1];

 // exitXpos: the X pixel position (not <<CSFed) of the frame of the exit
 // door. when walking out the door, keen's sprite will not be drawn past
 // this point.
 unsigned int level_done, level_done_timer;
 unsigned int level_finished_by;      // index of player that finished level
 unsigned int exitXpos;
  
 // stuff for custom episode's options
 struct
 {
 	char DoorOpenDir;
	char pShotSpeed;
	char VortDieDoFade;
	char vgatiles;
	char vgasprites;
 } cepvars;
} stLevelControl;

// things which can be "overlaid" on the game play
// if these values are nonzero, they are visible
typedef struct stOverlays
{
	int keenslefttime;		// if >0, "keens left" dialog is visible
	char statusbox, statusboxplayer;
	char youseeinyourmind;
	char uhoh;
	char debugmode;
	
	char endsequence;
	char tantalus;		// sequence which a tantalus ray switch is pushed
} stOverlays;

#define OPT_RENDERRATE			0		// render fps (default 60)
#define OPT_2BUTTONFIRING		1		// 1=standard ctrl+alt fire, 0=space fire
#define OPT_FULLYAUTOMATIC      2		// 1=fully automatic raygun
#define OPT_SUPERPOGO           3		// 1=pogo jumps really high
#define OPT_HARD                4		// 1=increase difficulty
#define OPT_ALLOWPKING          5		// 1=allow players to kill each other
#define OPT_CHEATS              6		// 1=enable clonekeen-specific cheats
#define OPT_FULLSCREEN          7		// 1=full screen mode
#define OPT_ZOOM                8		// 1=640x480 (2x) mode
#define OPT_ZOOMONRESTART       9		// what OPT_ZOOM will be after restart
#define OPT_MUSTKILLYORPS		10		// 1=must kill yorps to leave level
#define OPT_YORPFORCEFIELDS		11		// 1=must stun yorps to kill them
#define OPT_RISEBONUS			12		// 1=show rising bonus points
#define OPT_SHOWOSD				13		// 1=show OSD
#define OPT_SHOWSCORE			14		// 1=show score in OSD
#define OPT_SHOWCARDS			15		// 1=show cards in OSD
#define OPT_DOORSBLOCKRAY		16		// 1=can't shoot through locked doors
#define OPT_SOUNDOFFINDEMO		17		// no sound during demo
#define OPT_P1COLOR				18		// player 1 color
#define OPT_P2COLOR				19		// player 2 color
#define OPT_GHOSTING			20		// show object ghosts in level editor

#define NUM_OPTIONS             21

// the characters used as "checkmarks" in the option-setup menus
#define OPTION_ENABLED_CHAR		159
#define OPTION_DISABLED_CHAR	158


// if a demo is playing or being recorded,
// this struct, instantiated in "demo.", will provide
// more information about the demo
#define DEMO_MAX_SIZE            80000
typedef struct stDemo
{
	int current_demo;
	FILE *fp;
	unsigned int RLERunLen;
	unsigned char data[DEMO_MAX_SIZE+1];
	unsigned int data_index;
	int options[NUM_OPTIONS];
} stDemo;


#define MAP_MAXWIDTH		256
#define MAP_MAXHEIGHT		256
#define MAP_MINWIDTH		((WINDOW_WIDTH>>TILE_S)+10)
#define MAP_MINHEIGHT		((WINDOW_HEIGHT>>TILE_S)+10)
#define MIN_SCROLL_X		32
#define MIN_SCROLL_Y		32

#define OPTION_NOT_FORCED			0
#define OPTION_FORCE_ENABLE		1
#define OPTION_FORCE_DISABLE	2
typedef struct stMap
{
 unsigned int xsize, ysize;            // size of the map
 unsigned char isworldmap;             // if 1, this is the world map
 unsigned int mapdata[MAP_MAXWIDTH][MAP_MAXHEIGHT];       // the map data
 // in-game, contains monsters and special object tags like for switches 
 // on world map contains level numbers and flags for things like teleporters
 unsigned int objectlayer[MAP_MAXWIDTH][MAP_MAXHEIGHT];
  // player start pos
 int startx, starty;
 // if 1, there is a time limit to finish the level
 char hastimelimit;			
 int time_m, time_s;		// how much time they have
 // play Tantalus Ray cinematic on time out (ep2)
 // or Game Over on time out (all other episodes)
 char GameOverOnTimeOut;
 // map forced options (for usermaps)
 int forced_options[NUM_OPTIONS];
} stMap;

typedef struct stTile
{
	int solidfall;       // if =1, things can not fall through
	int solidl;          // if =1, things can not walk through left->right
	int solidr;          // if =1, things can not walk through right->left
	int solidceil;       // if =1, things can not go up through
	int goodie;          // if =1, is reported to get_goodie on touch
	int standgoodie;     // if =1, is reported to get_goodie when standing on it
	int lethal;          // if =1 and goodie=1, is deadly to the touch
	int pickupable;      // if =1, will be erased from map when touched
	int points;		  // how many points you get for picking it up
	int priority;        // if =1, will appear in front of objects
	int ice;             // if =1, it's very slippery!
	int semiice;         // if =1, player has no friction but can walk normally
	int masktile;        // if nonzero, specifies a mask for this tile
	int bonklethal;      // if you hit your head on it you die (hanging moss)
	int chgtile;         // tile to change to when level completed (for wm)
					  // or tile to change to when picked up (in-level)
	// stuff for animated tiles
	unsigned char isAnimated;  // if =1, tile is animated
	unsigned int animOffset;   // starting offset from the base frame
	unsigned int animlength;   // animation length
} stTile;

typedef struct stBitmap
{
	int xsize;
	int ysize;
	unsigned char *bmptr;
	char name[9];
} stBitmap;

typedef struct stSprite
{
	unsigned char xsize, ysize;
	unsigned char **imgdata;//[MAX_SPRITE_HEIGHT][MAX_SPRITE_WIDTH];
	unsigned char **maskdata;//[MAX_SPRITE_HEIGHT][MAX_SPRITE_WIDTH];
	// bounding box for hit detection
	unsigned int bboxX1, bboxY1;
	unsigned int bboxX2, bboxY2;
} stSprite;

// for strings loaded from "strings.dat"
#define MAX_STRINGS             100
#define MAX_ATTRIBUTES          16
typedef struct stString
{
	unsigned char *name;    // pointer to malloc'd area containing string name
	unsigned char *stringptr;    // pointer to malloc'd area containing string
	
	int numAttributes;
	unsigned char *attrnames[MAX_ATTRIBUTES+1];
	unsigned int attrvalues[MAX_ATTRIBUTES+1];
} stString;

// structures for each AI module's data
#include "ai/enemydata.h"

// and the object structure containing the union of the above structs
typedef struct stObject
{
	unsigned int type;        // yorp, vorticon, etc.
	unsigned int exists;
	unsigned int onscreen;    // 1=(scrx,scry) position is visible onscreen
	unsigned int hasbeenonscreen;
	unsigned int sprite;      // which sprite should this object be drawn with
	unsigned int x, y;        // x,y location in map coords, CSFed
	int scrx, scry;           // object's x,y pixel position on screen
	int dispx, dispy;		   // actual position object is displayed*
	//*this can be different from scrx,scry if the window is 2x scaled,
	// scrx,scry is always 1x screen, dispx,dispy is ACTUAL sprite pos in window
	
	// if 1 the enemy is dead. this is only implemented for some enemies so far.
	// it's used to calculate numyorps in the kill-the-yorps mode
	char dead;
	
	// if type is OBJ_PLAYER, this contains the player number that this object
	// is associated with
	int AssociatedWithPlayer;
	
	// if zero, priority tiles will not be honored and object will always
	// appear in front of the background
	char honorPriority;
	
	char canbezapped;         // if 1 can be hit by a raygun blast
	uint zapped;              // number of times has been hit by a raygun shot
	int zapx, zapy, zapd;	   // x,y, and direction of last shot at time of impact
	char zappedbyenemy;	   // if 1, it was an ENEMYRAY and not keen that shot it
	
	char inhibitfall;         // if 1 common_enemy_ai will not do falling
	char cansupportplayer[MAX_PLAYERS];
	
	unsigned int blockedl, blockedr, blockedu, blockedd;
	signed int xinertia, yinertia;
	unsigned char xinertiatimer, yinertiatimer;
	
	unsigned char touchPlayer;      // 1=hit detection found it touching player
	unsigned char touchedBy;        // which player was hit
	// Y position on this object the hit was detected
	// this is used for the yorps' bonk-on-the-head thing.
	// objects are scanned bottom to top, and first pixel
	// touching player is what goes in here.
	unsigned char hity;
	
	unsigned int needinit;    // 1=new object--requires initilization
	unsigned char wasoffscreen;  // set to 1 when object goes offscreen
	// data for ai and such, used differently depending on
	// what kind of object it is
	union ai
	{
		// ep1
		stYorpData yorp;
		stGargData garg;
		stVortData vort;
		stButlerData butler;
		stTankData tank;
		stRayData ray;
		stDoorData door;
		stIceChunk icechunk;
		stTeleportData teleport;
		stRopeData rope;
		// ep2
		stWalkerData walker;
		stPlatformData platform;
		stVortEliteData vortelite;
		stSEData se;
		stBabyData baby;
		// ep3
		stFoobData foob;
		stNinjaData ninja;
		stMeepData meep;
		stMotherData mother;
		stBallJackData bj;
		stNessieData nessie;
	} ai;
	
	//unsigned char erasedata[64][64];   // backbuffer to erase this object
} stObject;

// (map) stripe attribute structures, for animated tiles
// slot 0 is not used. data starts at slot 1. see description
// of AnimTileInUse in map structure to see why.
typedef struct stAnimTile
{
  int slotinuse;        // if 0, this entry should not be drawn
  int x;                // x pixel position in scrollbuf[] where tile is
  int y;                // y pixel position in scrollbuf[]
  int baseframe;        // base frame, i.e. the first frame of animation
  int offset;           // offset from base frame
} stAnimTile;

// ** objects from KEEN1
#define OBJ_YORP           1
#define OBJ_GARG           2
#define OBJ_VORT           3
#define OBJ_BUTLER         4
#define OBJ_TANK           5
#define OBJ_RAY            6     // keen's raygun blast
#define OBJ_DOOR           7     // an opening door
#define OBJ_ICECHUNK       8     // ice chunk from ice cannon
#define OBJ_ICEBIT         9     // piece of shattered ice chunk
#define OBJ_PLAYER         10	 // a player puppet object
#define OBJ_TELEPORTER     11    // world map teleporter effect
#define OBJ_ROPE           12	 // rope holding stone above final vorticon

// ** objects from KEEN2 (some of these are in ep3 as well)
#define OBJ_WALKER               13
#define OBJ_TANKEP2              14
#define OBJ_PLATFORM             15
#define OBJ_VORTELITE            16
#define OBJ_SECTOREFFECTOR       17
#define OBJ_BABY                 18
#define OBJ_EXPLOSION            19
#define OBJ_EARTHCHUNK           20
#define OBJ_SPARK				 21

// ** objects from KEEN3
#define OBJ_FOOB                 22
#define OBJ_NINJA                23
#define OBJ_MEEP                 24
#define OBJ_SNDWAVE              25
#define OBJ_MOTHER               26
#define OBJ_FIREBALL             27
#define OBJ_BALL                 28
#define OBJ_JACK                 29
#define OBJ_PLATVERT             30
#define OBJ_NESSIE               31

// the guns that shoot periodically
#define OBJ_AUTORAY				 32
#define OBJ_AUTORAY_V			 33
#define OBJ_ICECANNON			 34

#define OBJ_GOTPOINTS			 35	  //this thing is the rising point numbers
#define OBJ_GHOST				 36	  //ghosted object from map editor
#define LAST_OBJ_TYPE      		 37


// default sprites for objects...when an object is spawned it's sprite is
// set to one of these depending on it's type.
#define OBJ_YORP_DEFSPRITE			50
#define OBJ_GARG_DEFSPRITE			60
#define OBJ_VORT_DEFSPRITE_EP1		78
#define OBJ_VORT_DEFSPRITE_EP2		82
#define OBJ_VORT_DEFSPRITE_EP3		71
#define OBJ_VORT_JUMP_DEFSPRITE_EP3	73		// for vorticons that start jumping
#define OBJ_BUTLER_DEFSPRITE		88
#define OBJ_TANK_DEFSPRITE			98
#define OBJ_RAY_DEFSPRITE_EP1		108
#define OBJ_RAY_DEFSPRITE_EP2		122
#define OBJ_RAY_DEFSPRITE_EP3		102
#define OBJ_ICECHUNK_DEFSPRITE		112
#define OBJ_ICEBIT_DEFSPRITE     	113
#define OBJ_ROPE_DEFSPRITE       	114
#define OBJ_TELEPORTER_DEFSPRITE	180

#define OBJ_PLATFORM_DEFSPRITE_EP2 	126
#define OBJ_PLATFORM_DEFSPRITE_EP3	107
#define OBJ_WALKER_DEFSPRITE		102
#define OBJ_TANKEP2_DEFSPRITE		112
#define OBJ_VORTELITE_DEFSPRITE		88

#define OBJ_FOOB_DEFSPRITE			95
#define OBJ_NINJA_DEFSPRITE			77
#define OBJ_MOTHER_DEFSPRITE		87
#define OBJ_BALL_DEFSPRITE			109
#define OBJ_JACK_DEFSPRITE			110
#define OBJ_MEEP_DEFSPRITE			118
#define OBJ_NESSIE_DEFSPRITE		132

#define OBJ_BABY_DEFSPRITE_EP2		52
#define OBJ_BABY_DEFSPRITE_EP3		51

#define OBJ_SPARK_DEFSPRITE_EP2		128

// some directions (mostly for OBJ_ICECHUNK and OBJ_ICEBIT)
#define DUPRIGHT         0
#define DUPLEFT          1
#define DUP              2
#define DDOWN            3
#define DDOWNRIGHT       4
#define DDOWNLEFT        5
#define DLEFT            6
#define DRIGHT           7

// directions for OBJ_EARTHCHUNK
#define EC_UPLEFTLEFT       0             // 22 degrees CC of UP/LEFT
#define EC_UPUPLEFT         1             // 22 degrees C of UP/LEFT
#define EC_UP               2             // straight UP
#define EC_UPUPRIGHT        3             // 22 degrees CC of UP/RIGHT
#define EC_UPRIGHTRIGHT     4             // 22 degrees C of UP/RIGHT
#define EC_DOWNLEFTLEFT     5             // 22 degrees CC of DOWN/LEFT
#define EC_DOWNDOWNLEFT     6             // 22 degrees C of DOWN/LEFT
#define EC_DOWN             7             // straight DOWN
#define EC_DOWNDOWNRIGHT    8             // 22 degrees CC of DOWN/RIGHT
#define EC_DOWNRIGHTRIGHT   9             // 22 degrees C of DOWN/RIGHT

#define EC_UPLEFT           10
#define EC_UPRIGHT          11
#define EC_DOWNLEFT         12
#define EC_DOWNRIGHT        13

// scroll trigger positions
#define SCROLLTRIGGERRIGHT     (WINDOW_WIDTH-126)
#define SCROLLTRIGGERLEFT      110
#define SCROLLTRIGGERUP        80
#define SCROLLTRIGGERDOWN      (WINDOW_HEIGHT-96)

// this structure contains all the variables used by a player
typedef struct stPlayer
{
   // these coordinates are CSFed
   unsigned long x;
   signed int y;

   char isPlaying;
   int useObject;

   char godmode;

   char standingonexit;		// is 1 if the player is on top of an exit door
   
   // used on world map only --------
   char hideplayer;
   char mounted;
   signed int pinertia_y;

   // used to save player position on WM while in a level
   unsigned long mapplayx;
   signed int mapplayy;
   uchar mapdir;
   //--------------------------------
   
   int playframe;

   unsigned char pfalling,plastfalling,pfallspeed,pfallspeed_increasetimer;

   unsigned char pwalking,playspeed;
   unsigned char pwalkframe,pwalkframea,pwalkanimtimer;
   unsigned char pwalkincreasetimer, pfriction_timer_x, pfriction_timer_y;
   signed int pinertia_x,playpushed_x;
   unsigned char playpushed_decreasetimer;

   unsigned char blockedl,blockedr,blockedu,blockedd;
   unsigned int blockedby;

   unsigned char pjumping, pjumptime, pjumpupspeed_decreasetimer;
   unsigned char pjumpframe, pjumpanimtimer, pjumpupspeed;
   unsigned char pjumpnormaltime, pjumpupdecreaserate, pjustjumped;
   unsigned char pjustfell;
   unsigned char pjumpfloattimer;

   unsigned char pdir,pshowdir,lastpdir;

   char pfiring,pfireframetimer;
   char inhibitwalking, inhibitfall;

   int ctrltimer, alttimer;
   char keyprocstate;
   char wm_lastenterstate;

   char pdie, pdieframe, pdietimer;
   int pdietillfly;
   signed int pdie_xvect;
   int psupportingtile, psupportingobject, lastsupportingobject;
   char psliding;
   char psemisliding;
   char ppogostick;
   int pfrozentime,pfrozenframe,pfrozenanimtimer;

   unsigned char keytable[PKEYTABLE_SIZE];
   unsigned char lastkeytable[PKEYTABLE_SIZE];
   unsigned char dpadcount, dpadlastcount;

   unsigned int ankhtime, ankhshieldobject;

	struct
	{
		unsigned long score;
		unsigned long extralifeat;
		unsigned int charges;        // ray gun ammo
		signed int lives;
		unsigned char HasPogo;
		unsigned char HasCardYellow;
		unsigned char HasCardRed;
		unsigned char HasCardGreen;
		unsigned char HasCardBlue;
		// only relevant for ep1
		unsigned char HasJoystick;
		unsigned char HasFuel;
		unsigned char HasBattery;
		unsigned char HasVacuum;
	} inventory;

} stPlayer;

#define TILE_LITTLE_DONE     77
#define TILE_BIG_DONE1       78
#define TILE_BIG_DONE2       79
#define TILE_BIG_DONE3       80
#define TILE_BIG_DONE4       81

#define TILE_TELEPORTER_GREY_IDLE  99
#define TILE_TELEPORTER_RED_INUSE  338

// special level codes on worldmap
#define LVLS_TELEPORTER_BONUS      46            // bonus teleporter in ep1
#define LVLS_SHIP                  20

#define TELEPORTING_OUT         0
#define TELEPORTING_IN          1

#define TELEPORT_BONUS_DESTX    ((((23085>>CSF>>TILE_S)+2)<<TILE_S<<CSF)-(8<<CSF))
#define TELEPORT_BONUS_DESTY    (((12501>>CSF>>TILE_S)+2)<<TILE_S<<CSF)

#define BONUSLEVEL_RESPAWN_X    31812
#define BONUSLEVEL_RESPAWN_Y    18936

#define TILE_SWITCH_UP             480
#define TILE_SWITCH_DOWN           493
#define TILE_LIGHTSWITCH           271
#define TILE_EXTENDING_PLATFORM    270

// "Sector Effector" types
#define SE_EXTEND_PLATFORM      1
#define SE_RETRACT_PLATFORM     2
#define SE_ANKHSHIELD           3
#define SE_MORTIMER_ARM         4
#define SE_MORTIMER_LEG_LEFT    5
#define SE_MORTIMER_LEG_RIGHT   6
#define SE_MORTIMER_SPARK       7
#define SE_MORTIMER_HEART       8
#define SE_MORTIMER_ZAPSUP      9
#define SE_MORTIMER_RANDOMZAPS  10

// animation rate of animated tiles
#define ANIM_TILE_TIME        50


#define TELEPORT_GRAY_BASEFRAME_EP1  342
#define TELEPORT_GRAY_IDLEFRAME_EP1  99

#define TELEPORT_RED_BASEFRAME_EP1   338
#define TELEPORT_RED_IDLEFRAME_EP1   325

#define TELEPORT_BASEFRAME_EP3  130
#define TELEPORT_IDLEFRAME_EP3  134

// special object markers
#define NESSIE_PATH             8192
#define NESSIE_PAUSE            8448
#define NESSIE_MOUNTPOINT       8704
#define ENEMY_STOPPOINT         1000
#define BALL_NOPASSPOINT        1001

#include "globals.h"

#endif