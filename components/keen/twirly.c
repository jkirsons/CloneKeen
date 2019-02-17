// sort of a C version of a "class" which
// provides a "twirly" marker, as used on the menus, etc.

#include "twirly.fdh"

#define TWIRL_SPEED				17
char twirler_frames[110] = {11,12,13,8,7,14,9,10};


void twirly_draw(int x, int y, int *frame)
{
	drawcharacter_clear(x, y, twirler_frames[*frame]);
}

void twirly_spin(int *frame, int *timer)
{
	if (*timer > TWIRL_SPEED)
	{
		*timer = 0;
		*frame = (*frame) + 1;
		if (!twirler_frames[*frame])
			*frame = 0;
	}
	else
	{
		*timer = (*timer) + 1;
	}
}

void twirly_init(int *frame, int *timer)
{
	*frame = 0;
	*timer = 0;
}
