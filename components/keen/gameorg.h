// various states we go through when a level is completed
// to do the walking out the exit door animation
#define LEVEL_NOT_DONE      0      // not completed
#define LEVEL_DONE_WALK     1      // walking through exit door
#define LEVEL_DONE_WAIT     2      // finished walk through door, wait a bit
#define LEVEL_DONE_FADEOUT  3      // fading out
#define LEVEL_COMPLETE      4      // on to the next level!

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
// in stage3 the shield flickers slow, no color change
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
// rate ate which the ice cannons in ep1 will fire
#define ICECANNON_FIRE_FREQ  400

// direction defines for various things
#define RIGHT    0
#define LEFT     1
#define UP       2
#define DOWN     3

#define ANKH_SHIELD_FRAME       61

// upon starting to walk, keen will quickly increase to
// PFASTINCMAXSPEED. keen can, at a slower rate,
// reach up to PMAXSPEED (increased every walk anim frame)
#define PFASTINCMAXSPEED  9
#define PMAXSPEED        13
#define PFASTINCRATE     16             // accel rate up to PFASTINCMAXSPEED
// rates at which player slows down
#define PFRICTION_RATE_INAIR     8
#define PFRICTION_RATE_ONGROUND  2
// rate at which player walking animation is shown
#define PWALKANIMRATE           40

// speed at which player walks through the exit door
#define PMAXEXITDOORSPEED  3

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
#define PJUMP_PREPARE_ANIM_RATE  15

// time to show player in his "pogo compressed" frame before jumping
#define PPOGO_PREPARE_TIME       60

#define PFIREFRAME               20     // raygun frame index
#define PFIRE_SHOWFRAME_TIME     100    // minimum time to show raygun frame
#define PFIRE_LIMIT_SHOT_FREQ    30     // maximum speed player can shoot

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
#define DIE_TILL_FLY_TIME   640
#define PDIE_RISE_SPEED    -20
#define DIE_MAX_XVECT       10

// initial negative inertia to provide player at
// start of a normal jump and a (high) pogo jump
#define PJUMPUP_SPEED            20
#define PPOGOUP_SPEED            24

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

// for short pogo jumps, uses -PJUMPUP_SPEED
#define PJUMP_NORMALTIME_POGO_SHORT       6
#define PJUMP_UPDECREASERATE_POGO_SHORT   6

// for high pogo jumps, uses -PPOGOUP_SPEED
//#define PJUMP_NORMALTIME_POGO_LONG        25
//#define PJUMP_UPDECREASERATE_POGO_LONG    12
#define PJUMP_NORMALTIME_POGO_LONG        36
#define PJUMP_UPDECREASERATE_POGO_LONG    6

// for the super-pogo option
#define PPOGOUP_SPEED_SUPER                     40
#define PJUMP_NORMALTIME_POGO_LONG_SUPER        10
#define PJUMP_UPDECREASERATE_POGO_LONG_SUPER    4

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
#define PFALL_MAXSPEED           16

// friction when player is pushed by yorp's, ball's, etc.
#define PLAYPUSH_DECREASERATE   1

// uses by the key processing function which changes CTRL, ALT, and CTRL+ALT
// into JUMP, POGO, and FIRE.
#define CTRLALT_DEBOUNCETIME    20

#define PDIEFRAME             22

// various tile indexes
#define BG_GRAY            143
#define BG_GRAY_PRIORITY   304
#define BG_BLACK           155
#define BG_DARK_EP2        147
#define TILE_FELLOFFMAP_EP1    582
#define TILE_FELLOFFMAP_EP3    0
#define TILE_ICE_LEFTCORNER   403
#define TILE_ICE_RIGHTCORNER  404
#define TILE_ICE              405
#define TILE_ICE_CUBE         394
#define TILE_GLOWCELL      241
#define DOOR_YELLOW        173
#define DOOR_RED           195
#define DOOR_GREEN         197
#define DOOR_BLUE          199
#define DOOR_YELLOW_EP3    234
#define DOOR_RED_EP3       236
#define DOOR_GREEN_EP3     238
#define DOOR_BLUE_EP3      240
#define DOOR_RED_SPRITE    (MAX_SPRITES-5)
#define DOOR_GREEN_SPRITE  (MAX_SPRITES-4)
#define DOOR_BLUE_SPRITE   (MAX_SPRITES-3)
#define DOOR_YELLOW_SPRITE (MAX_SPRITES-2)
#define YORPSTATUEHEAD     486
#define YORPSTATUEHEADUSED 485
#define YORPSTATUEBOTTOM   328
#define ENEMYRAY           109
#define ENEMYRAYEP2        123
#define ENEMYRAYEP3        103
#define RAY_VERT_EP3       104

// is player[].psupportingtile contains this value the player is actually
// supported by an object and you should look in player[].psupportingobj
// for it's index.
#define PSUPPORTEDBYOBJECT         0

