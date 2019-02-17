// Structs used for different enemy's variables. These are in a union.

typedef struct stYorpData
{
  unsigned char state;

  unsigned char looktimes,lookposition;
  unsigned char timer, dietimer;
  unsigned char walkframe;
  unsigned int dist_traveled;
  signed int yorpdie_inertia_y;

  unsigned char movedir;
} stYorpData;


typedef struct stGargData
{
  unsigned char state;

  unsigned char looktimes,lookframe;
  unsigned char timer, dietimer, keenonsameleveltimer;
  unsigned char about_to_charge;
  unsigned char walkframe;
  unsigned int dist_traveled;
  signed int gargdie_inertia_y;

  unsigned char movedir;
  unsigned char detectedPlayer, detectedPlayerIndex;
} stGargData;


typedef struct stVortData
{
  unsigned char state;

  unsigned char timer,timer2;
  unsigned int animtimer;
  unsigned char frame;
  unsigned int dist_traveled;
  signed int inertiay;

  unsigned char dir;
  // these hold the animation frames indexes since they're
  // different for each episode
  int WalkLeftFrame;
  int WalkRightFrame;
  int LookFrame;
  int JumpRightFrame;
  int JumpLeftFrame;
  int DyingFrame;
  int DeadFrame;
} stVortData;


// Vorticon Elite = the purple guys that shoot you in ep2
typedef struct stVortEliteData
{
  unsigned char state;

  unsigned char timer,timer2;
  unsigned int animtimer;
  unsigned char frame;
  signed int inertiay;
  unsigned char movedir;
  unsigned int timesincefire;

  int dist_traveled;
} stVortEliteData;


// Butler = the little pushing robots in ep1
typedef struct stButlerData
{
  unsigned char state;
  unsigned char timer,animtimer;
  unsigned char frame;
  unsigned int dist_traveled;

  unsigned char movedir;
} stButlerData;


// this same struct is used for both ep1 and ep2 "tanks", although
// they have seperate ai modules
typedef struct stTankData
{
  char state;

  uint timer,animtimer;
  uchar frame;
  uint dist_traveled;

  uchar movedir;

  uint ponsameleveltime;
  uchar alreadyfiredcauseonsamelevel;
  uchar fireafterlook;

  uchar detectedPlayer;         // 1 if player on same level
  uchar detectedPlayerIndex;    // index of player that was detected

  // for tank2
  uint timetillnextshot;
  uint firetimes;
  uint timetillcanfire;
  uint timetillcanfirecauseonsamelevel;
  uint turnaroundtimer;
  int pausetime;
} stTankData;


// a shot from keen's raygun or an enemy that shoots rayguns
// also used for various other types of projectiles
typedef struct stRayData
{
  char state;
  char direction;
  char zapzottimer;

  char dontHitEnable;
  int dontHit;         // index of an object type ray will not harm

  // for soundwave
  int animframe, animtimer;
  int offscreentime;

  // for earth chunks
  int baseframe;
} stRayData;


typedef struct stDoorData
{
	char timer;
} stDoorData;


typedef struct stIceChunk
{
	int vector_x, vector_y;
	int veloc_x, veloc_y;
} stIceChunk;


typedef struct stTeleportData
{
  char animtimer;
  char animframe;
  char numframechanges;

  char direction;
  int whichplayer;
  unsigned int destx;
  signed int desty;

  int baseframe;
  int idleframe;

  char NoExitingTeleporter;
  char snap;

  char fadeamt;
  char fadetimer;
} stTeleportData;


// the rope that you cut to kill the vorticon at the end of ep1
typedef struct stRopeData
{
	char state;
	int droptimer;
	int stoneX, stoneY;
	int bgtile;
} stRopeData;


typedef struct stWalkerData
{
  unsigned char state;

  unsigned char animtimer, dietimer;
  unsigned char walkframe;
  signed int walkerdie_inertia_y;
  int fallinctimer,fallspeed;

  unsigned char walkdir;
  unsigned char kickedplayer[MAX_PLAYERS];
} stWalkerData;


// a moving platform
typedef struct stPlatformData
{
  unsigned char state;
  unsigned char animframe;
  unsigned int animtimer;
  unsigned int waittimer;

  unsigned char movedir;
  unsigned char kickedplayer[MAX_PLAYERS];
} stPlatformData;


// many different assorted things, many sector-effectors affect tiles
// in the level instead of being a sprite. named for the similiarity
// to the object in duke3d
typedef struct stSEData
{
  unsigned int type;

  unsigned char state;
  unsigned int timer;
  unsigned int platx, platy;
  unsigned int bgtile;
  unsigned int dir;

  int counter,destroytiles;
  unsigned int frame;
  int mx,my;
  int blowx,blowy;
} stSEData;



typedef struct stBabyData
{
  char state;
  char dir;
  signed int inertia_x, inertia_y;
  int jumpdectimer, xdectimer;
  int jumpdecrate;
  int dietimer;

  char walkframe;
  int walktimer;
} stBabyData;



typedef struct stFoobData
{
  char state;
  char dir;

  int animframe, animtimer;
  int OnSameLevelTime;
  int OffOfSameLevelTime;
  int spooktimer;
  int SpookedByWho;
  int blockedtime;
} stFoobData;


// Ninja = the kung-fu bears in ep3
typedef struct stNinjaData
{
  char state;
  char dir;

  int animframe, animtimer;
  unsigned int timetillkick;

  signed int XInertia, YInertia;
  unsigned int XFrictionTimer, YFrictionTimer;
  unsigned int XFrictionRate, YFrictionRate;
  int KickMoveTimer;
  int isdying;
  int dietimer;
} stNinjaData;


// vorticon mother
typedef struct stMotherData
{
  char state;
  char dir;
  char hittimes;

  int animframe, animtimer;
  int timer;
} stMotherData;



typedef struct stMeepData
{
  char state;
  char dir;

  int animframe, animtimer;
  int timer;
} stMeepData;



typedef struct stBallJackData
{
  char dir;
  int animframe, animtimer;
  int speed;
} stBallJackData;



#define NESSIETRAILLEN   5
typedef struct stNessieData
{
  char state;
  char leftrightdir, updowndir;
  unsigned int baseframe;

  int tiletrailX[NESSIETRAILLEN+1];
  int tiletrailY[NESSIETRAILLEN+1];
  int tiletrailhead;

  char animframe, animtimer;
  unsigned int destx, desty;

  unsigned int pausetimer;
  unsigned int pausex, pausey;

  unsigned int mortimer_swim_amt;
  unsigned int mounted[MAX_PLAYERS];
} stNessieData;


