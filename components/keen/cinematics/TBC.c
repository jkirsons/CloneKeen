// Displays a "To Be Continued" box and then game-over's.
// Common to all 3 end sequences.

#include "../keen.h"
#include "TBC.fdh"


int TBC_AfterTimer;
#define TBC_AFTERDELAY		200
void ToBeContinued_Start(void)
{
int dlgX,dlgY,dlgW,dlgH;
char *str = "TO_BE_CONTINUED";
	lprintf("> ToBeContinued_Start()\n");
	dlgX = GetStringAttribute(str, "LEFT");
	dlgY = GetStringAttribute(str, "TOP");
	dlgW = GetStringAttribute(str, "WIDTH");
	dlgH = GetStringAttribute(str, "HEIGHT");
	message(getstring("TO_BE_CONTINUED"), dlgX, dlgY, dlgW, dlgH, 1, 0);
	TBC_AfterTimer = TBC_AFTERDELAY;
}

void ToBeContinued_Do(void)
{
	message_do();
	if (message_done())
	{
		if (TBC_AfterTimer==0)
		{
			message_dismiss();
			SetGameOver();
			TBC_AfterTimer = -1;
		}
		else TBC_AfterTimer--;
	}
}
