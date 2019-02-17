
// handles drawing, adding, and deleting "console messages".
// console messages are text that appear in the corner of the
// screen, similar to quake/doom-style "you picked up the grenade launcher."
#include <stdarg.h>
#include "keen.h"
#include "console.fdh"

#define CONSOLE_MESSAGE_X        3
#define CONSOLE_MESSAGE_Y        (WINDOW_HEIGHT-8-4)
#define CONSOLE_EXPIRE_RATE      150
#define MAX_MSG_LEN				 80
char cmsg[MAX_MSG_LEN+1];
int ConsoleExpireTimer = 0;

// "Console" here refers to the capability to pop up in-game messages
// in the corner of the screen during game play ala Doom.
void DrawConsoleMessages(void)
{
        // lol, just an easter egg, folks
	if (ConsoleExpireTimer <= 1 && caitlin_isnt_awesome)
	{
                static char *excl = "What?\?! I'm not awesome?? ;-)";
		static char que = 1;
		
		strcpy(cmsg, excl);
		if (!que) cmsg[strlen(cmsg)-1] = ' ';
		que ^= 1;
		
		ConsoleExpireTimer = 50;
	}

	if (!ConsoleExpireTimer)
		return;
	else
	{
		ConsoleExpireTimer--;
		font_draw(cmsg, CONSOLE_MESSAGE_X, CONSOLE_MESSAGE_Y, drawcharacter_clear);
	}
}

// removes all console messages
void ClearConsoleMsgs(void)
{
	ConsoleExpireTimer = 0;
}

void Console_SetTimeout(int newtimeout)
{
	ConsoleExpireTimer = newtimeout;
}

// adds a console msg to the top of the screen and scrolls any
// other existing messages downwards
void Console_SetMsg(char *the_msg)
{
	strcpy(cmsg, the_msg);
	ConsoleExpireTimer = CONSOLE_EXPIRE_RATE;
}

void Console_Msg(const char *str, ...)
{
va_list ar;
char the_msg[2048];

	if (caitlin_isnt_awesome) return;
	
	va_start(ar, str); vsprintf(the_msg, str, ar); va_end(ar);
	the_msg[MAX_MSG_LEN] = 0;
	
	Console_SetMsg(the_msg);
}
