/* SEQ_EP3ENDING.C
  Ending cinematic sequence for Episode 3.
*/

#include "../keen.h"
#include "e3ending.fdh"

#define TAKING_PICTURES			0
#define THE_END					2
char seqstate3 = -1;


void seq_ep3ending_go(void)			// start the end sequence
{
	lprintf("> starting ep3 end sequence\n");
	overlay.endsequence = 1;
	
	TakingPictures_Start();
}

void seq_ep3ending_run(void)
{
	switch(seqstate3)
	{
		case TAKING_PICTURES: TakingPictures_Do(); break;
		case THE_END: TheEnd_Do(); break;
		
		default: crash("seq_ep1ending_run: invalid seqstate %d\n", seqstate3);
	}
}


int cameratime, camerabursttime, cameraburstcount;
#define CAMERA_RATE				100
#define CAMERA_BURST_RATE		40
char cur_part, disp_part;
int flash_no, last_flash_no;
int fadecounter;

// initilize the "Taking Pictures" scene, in which Keen is awarded the
// "Big V" in front of a bunch of vorticon journalists
char fadestarted;
void TakingPictures_Start(void)
{
int i;
	lprintf("> TakingPictures_Start()\n");

	showmapatpos(81, 32,32);

	// place keen in correct position
	map.startx = 244<<CSF;
	map.starty = 136<<CSF;
	
	// locate the vorticon's camera's
	find_cameras();
	
	levelcontrol.dontscroll = 1;
	fadestarted = 0;
	
	cur_part = 1;
	disp_part = 1;
	flash_no = last_flash_no = -1;

	seqstate3 = TAKING_PICTURES;
	cameratime = CAMERA_RATE/2;
	camerabursttime = 0;
	cameraburstcount = 0;
	fadecounter = 0;
	fade(FADE_IN, FADE_SLOW);
	fadecontrol.fadeout_complete = 0;
	srnd(47);
}

void TakingPictures_nextpage(void)
{
	cur_part++;
	if (cur_part==5)
	{
		fadecounter = 1000;
	}
	else disp_part = 1;
}

typedef struct
{
	int x, y;
	int flashtile, noflashtile;
} stCamera;
stCamera camera[50];
int num_cameras;

void static find_cameras(void)
{
int x,y;
	num_cameras = 0;
	for(y=0;y<map.ysize;y++)
	for(x=0;x<map.xsize;x++)
	{
		if (map.mapdata[x][y]==72 || map.mapdata[x][y]==61)
		{
			camera[num_cameras].x = x; camera[num_cameras].y = y;
			camera[num_cameras].noflashtile = map.mapdata[x][y];
			camera[num_cameras].flashtile = (map.mapdata[x][y]==72)?58:60;
			num_cameras++;
		}
	}
	lprintf("find_cameras: found %d vorticon cameras\n", num_cameras);
}

void static stop_flashing(void)
{
	if (flash_no != -1)
	{
	 	map_chgtile(camera[flash_no].x, camera[flash_no].y, camera[flash_no].noflashtile);
		flash_no = -1;
	}
}

void static take_picture(void)
{
	stop_flashing();

	if (!fade_in_progress() || fadecontrol.dir==FADE_FLASH) 
	{
		// pick a random vorticon to take a picture
		do { flash_no = rnd()%num_cameras; } while (flash_no==last_flash_no);
		last_flash_no = flash_no;
		map_chgtile(camera[flash_no].x, camera[flash_no].y, camera[flash_no].flashtile);
		gamedo_RenderScreen();
		
		fade(FADE_FLASH, FADE_FAST);			
		sound_play(SOUND_GUN_CLICK, PLAY_NOW);
	}
}

void TakingPictures_Do(void)
{
char tempbuf[40];
	if (disp_part)
	{
		sprintf(tempbuf, "EP3_ESEQ_PAGE%d", cur_part);
		dispmsgstring(tempbuf, 1);
		message_SetDismissalCallback(TakingPictures_nextpage);
		disp_part = 0;
	}
	message_do();
	
	if (++cameratime >= CAMERA_RATE)
	{
		if (cameratime==CAMERA_RATE && rnd()&1)
		{
			cameratime = 0;
		}
		else
		{
			if (++camerabursttime > CAMERA_BURST_RATE)
			{
				camerabursttime = 0;
				if (rnd() & 1)
				{
					take_picture();
					if (++cameraburstcount==3)
					{
						cameraburstcount = 0;
						cameratime = 0;
					}
				}
			}
		}
	}
	
	if (!fade_in_progress() || fadecontrol.dir != FADE_FLASH)
		stop_flashing();
	
	
	if (fadecounter)
	{
		if (--fadecounter==0)
		{
			fadecontrol.fadeout_complete =0;
			fade(FADE_OUT, FADE_SLOW);
		}
	}
	else
	{
		if (fadecontrol.fadeout_complete)
		{
			TheEnd_Start();
		}
	}
}


char the_end_state;
#define STATE_WAITFADEIN		1
#define STATE_TIMEDELAY			0
#define STATE_WAITTYPEWRITER	2
#define STATE_FLASH				3

void TheEnd_Start(void)
{
int i;
char pagename[80];
	lprintf("> TheEnd_Start()\n");

	initgame();
	finale_draw("finale.ck3");
	map.xsize = map.ysize = 5;

	player[0].x = 16;
	player[0].y = 16;
	player[0].playframe = BLANKSPRITE;

	the_end_state = STATE_WAITFADEIN;
	
	seqstate3 = THE_END;
	levelcontrol.dontscroll = 1;
	fade(FADE_IN, FADE_SLOW);
}

int flash_timer;
char the_end_num;
#define FLASH_RATE		100			// rate question mark flashes
void TheEnd_StartFlashing(void)
{
	the_end_num = 1;
	flash_timer = FLASH_RATE;
	the_end_state = STATE_FLASH;
	last_keytable[KENTER] = 1;
}

int time_delay;
void TheEnd_Do(void)
{
char endstr[30];
int dlgX,dlgY,dlgW,dlgH;

	if (the_end_state==STATE_WAITFADEIN)
	{
		time_delay = 500;
		the_end_state = STATE_TIMEDELAY;
	}
	else if (the_end_state==STATE_TIMEDELAY)
	{
		if (!time_delay--)
		{
			dispmsgstring("THE_END_1", 1);
			message_SetDoneCallback(TheEnd_StartFlashing);
			the_end_state = STATE_WAITTYPEWRITER;
		}
	}
	else if (the_end_state==STATE_FLASH)
	{
		if (keytable[KENTER] && !last_keytable[KENTER])
		{
			SetGameOver();
			goto message_redraw;	// because ENTER dismissed it
		}
		if (!flash_timer--)
		{
			the_end_num = (the_end_num==1)?2:1;
message_redraw: ;
			flash_timer = FLASH_RATE;
			sprintf(endstr, "THE_END_%c", the_end_num+'0');
		    dlgX = GetStringAttribute(endstr, "LEFT");
		    dlgY = GetStringAttribute(endstr, "TOP");
		    dlgW = GetStringAttribute(endstr, "WIDTH");
		    dlgH = GetStringAttribute(endstr, "HEIGHT");
			message(getstring(endstr), dlgX, dlgY, dlgW, dlgH, 0,0);
		}
	}
	
	message_do();
}
