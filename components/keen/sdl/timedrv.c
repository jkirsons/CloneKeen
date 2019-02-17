/* The timer (speed throttler) driver for SDL.
   This code is a slightly modified version
   of the code used in FCEU.
*/

#include "../keen.h"
#include "timedrv.fdh"

ulong LastRenderTime, LastLogicTime, LastSecTime;
ulong RenderRate = (1000 / 60);			// 60fps
ulong LogicRate = (1000 / 333);			// 333fps

char TimeDrv_Start(void)
{
  lprintf("Starting timer driver...\n");
  TimeDrv_InitTimers();
  return 0;
}

void TimeDrv_Stop(void)
{
}

void TimeDrv_InitTimers(void)
{
	LastRenderTime = LastLogicTime = LastSecTime = SDL_GetTicks();
}

char TimeDrv_TimeToRunLogic(void)
{
ulong CurTime = SDL_GetTicks();
	if ((CurTime - LastLogicTime) >= LogicRate)
	{
		LastLogicTime += LogicRate;
		return 1;
	}
	else return 0;
}

char TimeDrv_TimeToRender(void)
{
ulong CurTime = SDL_GetTicks();
	if ((CurTime - LastRenderTime) >= RenderRate)
	{
		///LastRenderTime += RenderRate;
		LastRenderTime = CurTime;
		// do not render if we are behind on the logic
		if ((CurTime - LastLogicTime) > LogicRate)
		{
			//lprintf(">EEEEK!!!!!!!!! %d        %d\n", (CurTime-LastLogicTime),LogicRate);
			return 0;
		}
		
		if (options[OPT_RENDERRATE] > 0)
			RenderRate = (1000 / options[OPT_RENDERRATE]);
		return 1;
	}
	else return 0;
}

void TimeDrv_ResetSecondsTimer(void)
{
	LastSecTime = SDL_GetTicks();
}

// will return nonzero once per second
char TimeDrv_HasSecElapsed(void)
{
ulong CurTime = SDL_GetTicks();
	if (CurTime - LastSecTime >= 1000)
	{
		LastSecTime = CurTime;
		return 1;
	}
	return 0;
}
