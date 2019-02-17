#include "../keen.h"
#include "door.fdh"

// "AI" for the door object (to do the animation when a door
// opens, the door tiles are removed and replaced with a sprite
// that looks exactly the old tiles, which then moves down into
// the floor)

#define DOOR_OPEN_SPEED    10

void door_ai(int o)
{
	if (objects[o].needinit)
	{
		objects[o].ai.door.timer = 0;
		sprites[objects[o].sprite].ysize = 32;
		objects[o].inhibitfall = 1;
		objects[o].needinit = 0;
	}

   if (objects[o].ai.door.timer > DOOR_OPEN_SPEED)
   {
		if (levelcontrol.cepvars.DoorOpenDir==DOWN) objects[o].y += (1<<CSF);
		if (!--sprites[objects[o].sprite].ysize)
		{
			delete_object(o);
		}
		objects[o].ai.door.timer = 0;
   }
   else objects[o].ai.door.timer++;
}

