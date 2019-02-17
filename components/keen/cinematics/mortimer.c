// Handles the conversation with mortimer at beginning of final level
#include "../keen.h"
#include "mortimer.fdh"

void seq_mortimer_showpage1(void)
{
	gamedo_enemyai();	// initilizes meeps so they're visible during conversation
	dispmsgstring("EP3_MORTIMER_1", 1);
	message_SetDismissalCallback(seq_mortimer_showpage2);
	sound_stop_all();
	sound_play(SOUND_MORTIMER, PLAY_FORCE);
}

void seq_mortimer_showpage2(void)
{
	dispmsgstring("EP3_MORTIMER_2", 1);
	message_SetDismissalCallback(seq_mortimer_showpage3);
}

void seq_mortimer_showpage3(void)
{
	dispmsgstring("EP3_MORTIMER_3", 1);
	message_SetDismissalCallback(seq_mortimer_showpage4);
}

void seq_mortimer_showpage4(void)
{
	dispmsgstring("EP3_MORTIMER_4", 1);
	message_SetDismissalCallback(seq_mortimer_showpage5);
}

void seq_mortimer_showpage5(void)
{
	dispmsgstring("EP3_MORTIMER_5", 1);
	message_SetDismissalCallback(seq_mortimer_makefootslamnoise);
}

void seq_mortimer_makefootslamnoise(void)
{
	sound_play(SOUND_FOOTSLAM, PLAY_NOW);
}

