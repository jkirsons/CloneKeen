#include "../keen.h"
#include "gotpoints.fdh"

// GotPoints object (rising numbers when you get a bonus item)
// (this wasn't in original Keen)

#define GOTPOINTS_SPEED         4
#define GOTPOINTS_LIFETIME      100
#define YORPSHIELD_LIFETIME     20

void gotpoints_ai(int o)
{

	if (objects[o].needinit)
	{
		if (objects[o].sprite != YORPSHIELD_SPRITE)
			objects[o].ai.ray.offscreentime = GOTPOINTS_LIFETIME;
		else
			objects[o].ai.ray.offscreentime = YORPSHIELD_LIFETIME;
			
		objects[o].inhibitfall = 1;
		objects[o].honorPriority = 0;
		objects[o].needinit = 0;
	}

	// delete it after it's existed for a certain amount of time
	if (!objects[o].ai.ray.offscreentime)
	{
		delete_object(o);
		return;
	}
	else objects[o].ai.ray.offscreentime--;

	// rise up
	if (objects[o].sprite != YORPSHIELD_SPRITE)
	{
		objects[o].y -= GOTPOINTS_SPEED;
	}
}

