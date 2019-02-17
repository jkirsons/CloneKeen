// contains stuff used for cinematic sequences

#include "../keen.h"
#include "seqcommon.fdh"

// start the end sequence for whatever episode we're playing
void endsequence_start(void)
{
	if (editor) { crash("end sequence started from within level editor! report this problem!"); }
	
	sound_stop_all();
	numplayers = 1;
	switch(levelcontrol.episode)
	{
		case 1: seq_ep1ending_go(); break;
		case 3: seq_ep3ending_go(); break;
		default:
			crash("endsequence_start: no end sequence for episode # %d\n", levelcontrol.episode);
	}
}

void cinematics_stop(void)
{
	overlay.endsequence = 0;
	overlay.tantalus = 0;
}

// return 1 if any cinematic sequences are playing
char cinematic_is_playing(void)
{
int yes = 0;
	yes |= overlay.endsequence;
	yes |= overlay.tantalus;
	return yes;
}

// check if any cinematics are running, and if so play them.
// returns nonzero if any are playing
char cinematics_play(void)
{
	if (cinematic_is_playing())
	{
		// freeze sequence's logic state during fade in
		if (fade_in_progress() && fadecontrol.dir==FADE_IN) return 1;
	}
	else return 0;	// nothing is playing so no reason to go on

	
	if (overlay.endsequence)
	{		
		switch(levelcontrol.episode)
		{
			case 1: seq_ep1ending_run(); break;
			case 3: seq_ep3ending_run(); break;
			default:
				crash("cinematics_play: invalid eseq episode # %d\n", levelcontrol.episode);
		}
	}
	else if (overlay.tantalus)
	{
		seq_tantalus_run();
	}
	
	return 1;
}


void showmapatpos(int level, int xoff, int yoff)
{
int i;
char levelname[80];
	lprintf("showmapatpos(%d, %d, %d, %d);\n",level,xoff,yoff);

	if (loadlevel(level))
		{ crash("showmapatpos: unable to load level %d\n", level); return; }
	
	for(i=0;i<yoff;i++) map_scroll_down();
	for(i=0;i<xoff;i++) map_scroll_right();
}
