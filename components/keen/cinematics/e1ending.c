/* SEQ_EP1ENDING.C
  Ending cinematic sequence for Episode 1.
*/

#include "../keen.h"
#include "e1ending.fdh"

#define CMD_MOVE                0
#define CMD_WAIT                1
#define CMD_SHOWSPRITE          2
#define CMD_HIDESPRITE          3
#define CMD_FADEOUT             4
#define CMD_ENDOFQUEUE          5
#define CMD_ENABLESCROLLING     6
#define CMD_DISABLESCROLLING    7
typedef struct stShipQueue
{
  int cmd;
  int time;
  int flag;
} stShipQueue;
stShipQueue shipqueue[32];
int ShipQueuePtr;

// start x,y map scroll position for eseq1_ShipFlys()
#define SHIPFLY_X       0
#define SHIPFLY_Y       32

// worldmap scroll position for eseq1_ReturnsToShip()
#define BACKTOSHIP_X            8
#define BACKTOSHIP_Y            488

#define LETTER_SHOW_SPD          30
#define RETURNTOSHIP_WAIT_TIME   600

#define SPR_SHIP_RIGHT  115
#define SPR_SHIP_LEFT   116
#define SPR_EXCLAMATION 117
#define SPR_QUESTION    118
#define SHIPSPD         4

#define BACKHOME_SHORT_WAIT_TIME   250

#define BACK_AT_HIS_SHIP		0
#define SCENE_FLY_HOME			1
#define BACK_AT_HOME			2
#define TBC						3
char seqstate1 = -1;


void seq_ep1ending_go(void)			// start the end sequence
{
	lprintf("> starting ep1 end sequence\n");
	overlay.endsequence = 1;
	
	BackAtHisShip_Start();
}

void seq_ep1ending_run(void)
{
	switch(seqstate1)
	{
		case BACK_AT_HIS_SHIP: BackAtHisShip_Do(); break;
		case SCENE_FLY_HOME: FlyHome_Do(); break;
		case BACK_AT_HOME: BackAtHome_Do(); break;
		case TBC: ToBeContinued_Do(); break;
		
		default: crash("seq_ep1ending_run: invalid seqstate1 %d\n", seqstate1);
	}
}


// initilize the "back at his ship" scene, where keen returns and replaces
// the missing parts
char bahs_fadestarted;
void BackAtHisShip_Start(void)
{
int i;
	lprintf("> BackAtHisShip_Start()\n");

	// make all levels non-completed
	memset(levelcontrol.levels_completed, 0, sizeof(levelcontrol.levels_completed));

	showmapatpos(80, BACKTOSHIP_X, BACKTOSHIP_Y);
  
	// place keen next to his ship
	player[0].mapplayx = 208 << CSF;
	player[0].mapplayy = 625 << CSF;
	player[0].mapdir = LEFT;
	
	message(getstring("EP1_ESEQ_PART1"), 1*8, 23*8, 37, 6, 1, 0);

	levelcontrol.dontscroll = 1;
	bahs_fadestarted = 0;

	seqstate1 = BACK_AT_HIS_SHIP;
	fade(FADE_IN, FADE_SLOW);
}

void BackAtHisShip_Do(void)
{
	message_do();
			
	if (message_done())
	{
		if (!fade_in_progress())
		{
			if (!bahs_fadestarted)
			{
				fade(FADE_OUT, FADE_NORM);
				bahs_fadestarted = 1;
			}
			else
			{
				message_dismiss();
				FlyHome_Start();
			}
		}
	}
}


void addshipqueue(int cmd, int time, int flag)
{
	shipqueue[ShipQueuePtr].cmd = cmd;
	shipqueue[ShipQueuePtr].time = time;
	shipqueue[ShipQueuePtr].flag = flag;
	ShipQueuePtr++;
}

// initilize the STATE_FLY_HOME scene, which shows keen flying home from Mars,
// and discovering the alien mothership in orbit.
int marksprite_index, ship_index;
void FlyHome_Start(void)
{
int x, y;
	//scrollingon = 1;
	lprintf("> FlyHome_Start()\n");

	showmapatpos(81, SHIPFLY_X, SHIPFLY_Y);
	
	// set up the ship's route
	ShipQueuePtr = 0;
	addshipqueue(CMD_MOVE, 230, DUP);
	addshipqueue(CMD_WAIT, 50, 0);
	addshipqueue(CMD_MOVE, 2690, DDOWNRIGHT);
	addshipqueue(CMD_WAIT, 100, 0);
	addshipqueue(CMD_MOVE, 480, DDOWN);
	addshipqueue(CMD_WAIT, 150, 0);
	addshipqueue(CMD_SHOWSPRITE, 0, SPR_QUESTION);
	addshipqueue(CMD_DISABLESCROLLING, 0, 0);
	addshipqueue(CMD_WAIT, 350, 0);
	addshipqueue(CMD_HIDESPRITE, 0, 0);
	addshipqueue(CMD_WAIT, 50, 0);
	addshipqueue(CMD_MOVE, 660, DLEFT);
	addshipqueue(CMD_WAIT, 150, 0);
	addshipqueue(CMD_SHOWSPRITE, 0, SPR_EXCLAMATION);
	addshipqueue(CMD_WAIT, 500, 0);
	addshipqueue(CMD_HIDESPRITE, 0, 0);
	addshipqueue(CMD_WAIT, 50, 0);
	addshipqueue(CMD_MOVE, 660, DRIGHT);
	addshipqueue(CMD_WAIT, 25, 0);
	addshipqueue(CMD_ENABLESCROLLING, 0, 0);
	addshipqueue(CMD_MOVE, 465, DDOWN);
	addshipqueue(CMD_FADEOUT, 0, 0);
	addshipqueue(CMD_MOVE, 100, DDOWN);
	addshipqueue(CMD_ENDOFQUEUE, 0, 0);
	ShipQueuePtr = 0;

	// have the ship start at the center of Mars
	if (map_findtile(593, &x, &y))
	{ // found the tile at the center of mars
		player[0].x = ((x<<4)+1)<<CSF; 
		player[0].y = ((y<<4)-3)<<CSF; 
	}
	else
	{
		crash("FlyHome_Start(): unable to find the center of Mars.");
		return;
	}

	ship_index = spawn_object(player[0].x, player[0].y, OBJ_GOTPOINTS);
	objects[ship_index].sprite = SPR_SHIP_RIGHT;
	objects[ship_index].onscreen = 1;
	player[0].hideplayer = 1;

	levelcontrol.dontscroll = 0;

	seqstate1 = SCENE_FLY_HOME;
	fade(FADE_IN, FADE_NORM);
}


// play the STATE_FLY_HOME scene, which shows keen flying home from Mars,
// and discovering the alien mothership in orbit.
void FlyHome_Do(void)
{
	// execute current command in the queue
	switch(shipqueue[ShipQueuePtr].cmd)
	{
		case CMD_MOVE:		// move the ship
			switch(shipqueue[ShipQueuePtr].flag)
			{
				case DUP:
					player[0].y -= SHIPSPD;
					objects[ship_index].sprite = SPR_SHIP_RIGHT;
				break;
				case DDOWN:
					player[0].y += SHIPSPD/2;
					objects[ship_index].sprite = SPR_SHIP_RIGHT;
				break;
				case DLEFT:
					player[0].x -= SHIPSPD;
					objects[ship_index].sprite = SPR_SHIP_LEFT;
				break;
				case DRIGHT:
					player[0].x += SHIPSPD;
					objects[ship_index].sprite = SPR_SHIP_RIGHT;
				break;
				case DDOWNRIGHT:
					player[0].x += SHIPSPD*2;
					player[0].y += SHIPSPD*0.8;
					objects[ship_index].sprite = SPR_SHIP_RIGHT;
				break;
			}
			objects[ship_index].x = player[0].x;
			objects[ship_index].y = player[0].y;
		break;
		
		case CMD_SHOWSPRITE:	// show exclamation/question mark sprite
			marksprite_index = spawn_object(player[0].x + (20<<CSF), \
											player[0].y - (10<<CSF), \
											OBJ_GOTPOINTS);
			objects[marksprite_index].sprite = shipqueue[ShipQueuePtr].flag;
			objects[marksprite_index].onscreen = 1;
		break;
			
		case CMD_HIDESPRITE:	// hide exclamation/question mark sprite
			delete_object(marksprite_index);
		break;
		
		case CMD_ENABLESCROLLING:  levelcontrol.dontscroll = 0; break;
		case CMD_DISABLESCROLLING: levelcontrol.dontscroll = 1; break;

		case CMD_WAIT: break;	// just pause until command is over

		case CMD_FADEOUT:
			fade(FADE_OUT, FADE_NORM);
		break;
		
		case CMD_ENDOFQUEUE:
			BackAtHome_Start();
		break;
	}
	
	if (shipqueue[ShipQueuePtr].time)
	{
		// decrease the time remaining for current command to execute
		shipqueue[ShipQueuePtr].time--;
	}
	else
	{  // go to next command
		ShipQueuePtr++;
	}

	
}


// the scene that shows keen's house after he sneaks back in
int dlgX, dlgY, dlgW, dlgH;
int timedelay;
char *bahtext[8], curtext;
#define DELAY_TIME		100
void BackAtHome_Start(void)
{
int i;
char pagename[80];
	lprintf("> BackAtHome_Start()\n");

	initgame();
	finale_draw("finale.ck1");

	player[0].x = 16;
	player[0].y = 16;
	player[0].hideplayer = 1;

	dlgX = GetStringAttribute("EP1_ESEQ_PART2_PAGE1", "LEFT") * 8;
	dlgY = GetStringAttribute("EP1_ESEQ_PART2_PAGE1", "TOP") * 8;
	dlgW = GetStringAttribute("EP1_ESEQ_PART2_PAGE1", "WIDTH");
	dlgH = GetStringAttribute("EP1_ESEQ_PART2_PAGE1", "HEIGHT");

	// get pointers to all the strings we're going to be using
	for(i=0;i<8;i++)
	{
		sprintf(pagename, "EP1_ESEQ_PART2_PAGE%d", i+1);
		bahtext[i] = getstring(pagename);
	}

	seqstate1 = BACK_AT_HOME;
	levelcontrol.dontscroll = 1;
	timedelay = 0;
	fade(FADE_IN, FADE_NORM);
}

void BackAtHome_Do(void)
{
	if (timedelay < DELAY_TIME) { timedelay++; return; }
	else if (timedelay==DELAY_TIME)
	{
		curtext = 0;
		message(bahtext[0], dlgX, dlgY, dlgW, dlgH, 1, 0);
		timedelay++;
	}
	
	message_do();
	if (message_done())
	{
		if (curtext >= 7)
		{
			ToBeContinued_Start();
			seqstate1 = TBC;
		}
		else message(bahtext[++curtext], dlgX, dlgY, dlgW, dlgH, 1, 0);
	}
}

