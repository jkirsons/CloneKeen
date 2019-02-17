
void drawcharacter_clear_inversepink(int xoff, int yoff, int f);
void drawcharacter_clear(int xoff, int yoff, int f);
void drawcharacter_clear_erasebg(int xoff, int yoff, int f);
void drawcharacter(int xoff, int yoff, int f);

// various states we go through when a level is completed
// to do the walking out the exit door animation
#define LEVEL_NOT_DONE      0      // not completed
#define LEVEL_DONE_WALK     1      // walking through exit door
#define LEVEL_DONE_WAIT     2      // finished walk through door, wait a bit
#define LEVEL_DONE_FADEOUT  3      // fading out
#define LEVEL_COMPLETE      4      // on to the next level!

#define PLAYER_STARTLIVES		4	// # of lives you start with

// time the "keens left" status box stays up
#define KEENSLEFT_TIME			400

// width of the player sprite, used for walking "behind" the exit door frame
#define PLAYERSPRITE_WIDTH      16

// at STOPWALKING time after starting the exit door animation,
// keen stops walking and we go to LEVEL_DONE_WAIT.
#define LEVEL_DONE_STOPWALKING_TIME      450
// at TOTAL_WAIT_TIME, LEVEL_DONE_FADEOUT is initiated.
#define LEVEL_DONE_TOTAL_WAIT_TIME       1000

// for ankh shield...
// stage1 is no flicker of the shield, changes colors
// in stage2 the shield flickers fast, changes colors
// in stage3 the shield flashes slow, no color change
#define ANKH_STAGE1_TIME    3000
#define ANKH_STAGE2_TIME    1500
#define ANKH_STAGE3_TIME    500
#define PLAY_ANKH_TIME      (ANKH_STAGE1_TIME+ANKH_STAGE2_TIME+ANKH_STAGE3_TIME)
// these correspond stage 1,2 and 3 respectively
#define ANKH_STATE_NOFLICKER    0
#define ANKH_STATE_FLICKERFAST  1
#define ANKH_STATE_FLICKERSLOW  2

// rate at which the periodically firing guns in ep3 will fire
#define GUN_FIRE_FREQ        800
// rate at which the ice cannons in ep1 will fire
#define ICECANNON_FIRE_FREQ  400

// direction defines used for various things
// important that RIGHT and LEFT are 0 and 1
#define RIGHT    0
#define LEFT     1
#define UP       2
#define DOWN     3

#define ANKH_SHIELD_FRAME       61

// bits in a byte of input data from a demo file
#define DMO_MASK_LEFT		0x01
#define DMO_MASK_RIGHT		0x02
#define DMO_MASK_JUMP		0x04
#define DMO_MASK_POGO		0x08
#define DMO_MASK_FIRE		0x10
#define DMO_MASK_END		0x80

// upon starting to walk, keen will quickly increase to
// PFASTINCMAXSPEED. keen can, at a slower rate,
// reach up to PMAXSPEED (increased every walk anim frame)
#define PFASTINCMAXSPEED   9
#define PMAXSPEED          13
#define PFASTINCRATE       16      // accel delay rate up to PFASTINCMAXSPEED
#define PFASTINCRATE_POGO  50      // rate when pogo stick is out
// rates at which player slows down (it's a delay so higher = slower)
#define PFRICTION_RATE_INAIR      9      //8
#define PFRICTION_RATE_ONGROUND   4      //2
#define PFRICTION_RATE_WM         1      // on world map
// rate at which player walking animation is shown
#define PWALKANIMRATE             40

// speed at which player walks through the exit door
#define PMAXEXITDOORSPEED       3

// the various jump states
#define PNOJUMP       0                 // not jumping
#define PPREPAREJUMP  1                 // doing the jump animation
#define PJUMPUP       2                 // jumping
#define PPREPAREPOGO  3                 // "pogo compressed" anim frame
#define PPOGOING      4                 // pogoing

// the different jumping frames. when CTRL is held down the player will
// go from frame PPREPAREJUMPFRAME to PJUMP_PREPARE_LAST_FRAME at a rate
// of PJUMP_PREPARE_ANIM_RATE until either CTRL is released or the player
// reaches the last frame. How far he got will select one of the various
// jump heights, defined below.
#define PPREPAREJUMPFRAME        8
#define PJUMP_PREPARE_LAST_FRAME 13
#define PJUMP_PREPARE_ANIM_RATE  11

// time to show player in his "pogo compressed" frame before jumping
#define PPOGO_PREPARE_TIME       60

#define PSTANDFRAME				 0		// standing, looking right
#define PFIREFRAME               20     // raygun frame index
#define PFIRE_SHOWFRAME_TIME     100    // minimum time to show raygun frame
#define PFIRE_LIMIT_SHOT_FREQ    30     // maximum speed player can shoot
#define PFIRE_LIMIT_SHOT_FREQ_FA 60     // fully automatic version

// player frame indexes for world map
#define PMAPRIGHTFRAME   32
#define PMAPDOWNFRAME    36
#define PMAPLEFTFRAME    40
#define PMAPUPFRAME      44

// player dieing states
#define PDIE_NODIE      0
#define PDIE_DYING      1
#define PDIE_DEAD       2
#define PDIE_FELLOFFMAP 3

#define DIE_ANIM_RATE       50
#define DIE_TILL_FLY_TIME   600
#define PDIE_RISE_SPEED    -30
#define DIE_MAX_XVECT       10

// initial negative inertia to provide player at
// start of a normal jump and a (high) pogo jump
#define PJUMPUP_SPEED            	20	// normal jump

#define PPOGO_LOWJUMP_SPEED		    19	// bouncing on pogo
#define PPOGO_HIGHJUMP_SPEED        24	// jump on pogo
#define PPOGO_SUPERHIGHJUMP_SPEED   40  // jump on pogo and SuperPogo on

// for short pogo jumps, uses -PJUMPUP_SPEED
#define PJUMP_NORMALTIME_POGO_SHORT       6
#define PJUMP_UPDECREASERATE_POGO_SHORT   6

// for high pogo jumps, uses -PPOGOUP_SPEED
#define PJUMP_NORMALTIME_POGO_LONG        36
#define PJUMP_UPDECREASERATE_POGO_LONG    6

// for super-high pogo jumps
#define PJUMP_NORMALTIME_POGO_LONG_SUPER        10
#define PJUMP_UPDECREASERATE_POGO_LONG_SUPER    4

// These control the various jump heights.
// one of these pairs is selected depending on how
// long the CTRL key was held down while Keen
// was preparing to jump. #1 causes the highest
// jump and they get lower from there.
// NORMAL_TIME is the amount of time keen will
// jump straight up at speed -PJUMPUP_SPEED.
// when expired his jump slows down at a rate
// of DECREASERATE.
#define PJUMP_NORMALTIME_1         5
#define PJUMP_UPDECREASERATE_1     6

#define PJUMP_NORMALTIME_2         5
#define PJUMP_UPDECREASERATE_2     5

#define PJUMP_NORMALTIME_3         3
#define PJUMP_UPDECREASERATE_3     3

#define PJUMP_NORMALTIME_4         2
#define PJUMP_UPDECREASERATE_4     2

#define PJUMP_NORMALTIME_5         40
#define PJUMP_UPDECREASERATE_5     2

#define PJUMP_NORMALTIME_6         0
#define PJUMP_UPDECREASERATE_6     0

// pogo frames
#define PFRAME_POGO           24
#define PFRAME_POGOBOUNCE     25

// frame and animation speed for frozen keen (ep1) and stunned keen (ep2&3)
#define PFRAME_FROZEN         28
#define PFROZENANIMTIME       100
// how long keen should stay frozen when hit by an ice chunk
#define PFROZEN_TIME          1000
#define PFROZEN_THAW          100

// when falling keen's Y inertia increases at INCREASERATE up to MAXSPEED
#define PFALL_INCREASERATE       3
#define PFALL_MAXSPEED           17

// friction when player is pushed by yorp's, ball's, etc.
#define PLAYPUSH_DECREASERATE   1

// uses by the key processing function which changes CTRL, ALT, and CTRL+ALT
// into JUMP, POGO, and FIRE.
#define CTRLALT_DEBOUNCETIME    20

#define PDIEFRAME             22

// various tile indexes
#define BG_GREY					143
#define BG_GREY_PRIORITY		304
#define BG_BLACK				155		// also "dark" bg tile in ep1
#define BG_DARK_EP2				147		// "dark" bg tile in ep2
#define BG_LIGHT_L				157		// light from left side of lamp
#define BG_LIGHT_R				158		// light from right side of lamp
#define TILE_LAMP				156
#define TILE_LAMPEP3			276
#define TILE_LAMPEP3UP			272
#define BG_DARK_EP2				147
#define TILE_FELLOFFMAP			582
#define TILE_FELLOFFMAP_EP3		0
#define TILE_ICE_LEFTCORNER		403
#define TILE_ICE_RIGHTCORNER	404
#define TILE_ICE				405
#define TILE_ICE_CUBE			394
#define TILE_GLOWCELL			241
#define TILE_RAYGUN				175
#define TILE_RAYGUN_GREENBG		311		// ep2 from level with cyan bg
#define TILE_POGO				176
#define TILE_JOYSTICK			221
#define TILE_BATTERY			237
#define TILE_VACUUM				241
#define TILE_FUEL				245
#define TILE_SOLID			1	// can be any tile which is solid in all 3 eps

#define TILE_ELDERSWITCH		429
#define TILE_ELDERSWITCH_USED	441

// ep1 & 2 keycard powerups
#define TILE_KEYYELLOW			190
#define TILE_KEYRED				191
#define TILE_KEYGREEN			192
#define TILE_KEYBLUE			193

// these are version of all powerups on a dark background,
// which are automatically created at runtime. Useful for
// user levels. Applies to episodes 1 & 2.
#define TILES_DARK_START		689
#define TILE_100PTS_DARK	 	689
#define TILE_200PTS_DARK		690
#define TILE_500PTS_DARK		691
#define TILE_1000PTS_DARK		692
#define TILE_5000PTS_DARK		693
#define TILE_KEYBLUE_DARK	 	694
#define TILE_KEYYELLOW_DARK	 	695
#define TILE_KEYGREEN_DARK	 	696
#define TILE_KEYRED_DARK	 	697
#define TILE_RAYGUN_DARK	 	698
#define TILES_DARK_LAST			698

#define DOOR_YELLOW				173
#define DOOR_RED				195
#define DOOR_GREEN				197
#define DOOR_BLUE				199
#define DOOR_YELLOW_EP3			234
#define DOOR_RED_EP3			236
#define DOOR_GREEN_EP3			238
#define DOOR_BLUE_EP3			240
#define YORPSTATUEHEADUSED		485
#define GARGSTATUEHEADUSED		434
#define ENEMYRAY				109
#define ENEMYRAYEP2				123
#define ENEMYRAYEP3				103
#define RAY_VERT_EP3			104

// special sprites (they either weren't in the game originally,
// or are used for internal engine stuff).
#define BLANKSPRITE			(MAX_SPRITES-1)
#define DOOR_YELLOW_SPRITE	(MAX_SPRITES-2)			// opening door (yellow)
#define DOOR_BLUE_SPRITE	(MAX_SPRITES-3)
#define DOOR_GREEN_SPRITE	(MAX_SPRITES-4)
#define DOOR_RED_SPRITE		(MAX_SPRITES-5)
#define PT5000_SPRITE		(MAX_SPRITES-6)			// rising bonus pts (+5000)
#define PT1000_SPRITE		(MAX_SPRITES-7)
#define PT500_SPRITE		(MAX_SPRITES-8)
#define PT200_SPRITE		(MAX_SPRITES-9)
#define PT100_SPRITE		(MAX_SPRITES-10)
#define PT1UP_SPRITE		(MAX_SPRITES-11)		// rising 1up bonus
#define PTCARDY_SPRITE		(MAX_SPRITES-12)		// rising access card
#define PTCARDR_SPRITE		(MAX_SPRITES-13)
#define PTCARDG_SPRITE		(MAX_SPRITES-14)
#define PTCARDB_SPRITE		(MAX_SPRITES-15)
#define SHOTUP_SPRITE		(MAX_SPRITES-16)		// rising single shot (ep3)
#define GUNUP_SPRITE		(MAX_SPRITES-17)		// rising ray gun
#define YORPSHIELD_SPRITE	(MAX_SPRITES-18)		// invincibility force field, used in editor and in "yorps have forcefields" mode
#define VERSION_SPRITE		(MAX_SPRITES-19)		// version text shown in lower-right corner at startup
#define DEMOBOX_SPRITE		(MAX_SPRITES-20)		// says "Demo"
#define OSD_LIVES_SPRITE	(MAX_SPRITES-21)
#define OSD_AMMO_SPRITE		(MAX_SPRITES-22)
#define OSD_YORPS_SPRITE	(MAX_SPRITES-23)
#define TITLE_LOGO1_SPRITE	(MAX_SPRITES-24)		// left half of the logo
#define TITLE_LOGO2_SPRITE	(MAX_SPRITES-25)		// right half of the logo
#define TITLE_FLOOR_SPRITE	(MAX_SPRITES-26)		// more stuff for the logo
#define TITLE_VORT_SPRITE   (MAX_SPRITES-27)
#define ARROWLR_SPRITE		(MAX_SPRITES-28)		// these are directional arrows used in the editor
#define ARROWUD_SPRITE		(MAX_SPRITES-29)
#define ARROWUR_SPRITE		(MAX_SPRITES-30)
#define ARROWUL_SPRITE		(MAX_SPRITES-31)
#define ARROWU_SPRITE		(MAX_SPRITES-32)
#define ARROWD_SPRITE		(MAX_SPRITES-33)
#define LAST_SPECIAL_SPRITE (MAX_SPRITES-34)

// coordinate at which to draw DEMOBOX_SPRITE
#define DEMOBOX_X			137
#define DEMOBOX_Y			16

// if player[].psupportingtile contains this value the player is actually
// supported by an object and you should look in player[].psupportingobj
// for it's index.
#define PSUPPORTEDBYOBJECT         0

