// code for "messages", which is text that appears over the game in a
// dialog box. these are most commonly used during ending sequences,
// "you see in your mind", etc.

#include "keen.h"
#include "message.fdh"

uchar *msg = NULL, *vismsg = NULL;
int msg_x, msg_y, msg_w, msg_h;
int showlen, maxlen;
int msg_timer;
int aftertimer;
char typewriter_mode, pausing=0;
void (*DrawCB)(void) = NULL;
void (*DoneCB)(void) = NULL;
void (*DoCB)(void) = NULL;
void (*DismissalCB)(void) = NULL;
#define TYPEWRITER_SPEED		40
#define AFTER_DELAY				900

// start displaying a message.
// newmsg:	a pointer to the message string to be displayed.
// x,y:		x&y, in pixels, of the UL corner of the box, relative to a 320x240
//			screen (automatically scaled x2 if screen is 640x480)
// w,h:		the width and height, in 8-pixel characters, of the dialog box
// typewriter:	if 1, letters appear one at a time, like a typewriter, and the
//				dialog automatically times-out when all letters are shown
// pause_game:	if 1, the game will be paused while the dialog is visible				
void message(char *newmsg, int x, int y, int w, int h, char typewriter,\
			 char pause_game)
{
int len = strlen(newmsg);

	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (w < 0) w = 20;
	if (h < 0) h = 3;
	if (!newmsg) newmsg = "NULL MESSAGE!";

	message_dismiss();
	maxlen = strlen(newmsg);

	vismsg = malloc(maxlen + 2);
	if (!vismsg) { crash("message: cannot allocate space for vismsg buffer!"); return; }

	if (typewriter)
	{
		msg = malloc(maxlen + 2);
		if (!msg) { crash("message: cannot allocate space for msg buffer!"); return; }

		strcpy(msg, newmsg);
		memset(vismsg, 0, maxlen+1);
		showlen = 0;
	}
	else
	{
		strcpy(vismsg, newmsg);
	}
	typewriter_mode = typewriter;
	pausing = pause_game;
	if (pausing) sound_pause();
	
	msg_x = x; msg_y = y;
	msg_w = w; msg_h = h;
	msg_timer = 0;
	aftertimer = 0;
	DrawCB = DoCB = DoneCB = DismissalCB = NULL;
	last_keytable[KENTER] = 1;
	last_keytable[KSPACE] = 1;
}

// if any messages are up, gets rid of them
void message_dismiss(void)
{
void (*funcptr)(void) = NULL;
	if (msg) { free(msg); msg = NULL; }
	if (vismsg) { free(vismsg); vismsg = NULL; }
	
	if (pausing)
	{
		pausing = 0;		// makes message_up_and_pauses simpler
		sound_resume();
	}

	if (DismissalCB)
	{
		// necessary to do it this way so as not to cause an endless
		// loop if the dismissal handler calls message() again.
		funcptr = DismissalCB;
		DismissalCB = NULL;
		(*funcptr)();
	}
}

// add a "draw extension", a callback which is called every time the message
// is drawn, which can be used to add special effects or special graphics
// to the box
void message_SetDrawCallback(void *fp)
{
	DrawCB = fp;
}
// call back the specified function when message_do is called
void message_SetDoCallback(void *fp)
{
	DoCB = fp;
}
// call back the specified function when the message is dismissed
void message_SetDismissalCallback(void *fp)
{
	DismissalCB = fp;
}
// call back the specified function when the message is completely displayed
void message_SetDoneCallback(void *fp)
{
	DoneCB = fp;
}

// draw any visible messages. this code runs at RENDER framerate
void DrawMessages(void)
{
	if (!vismsg) return;
	dialogbox(msg_x, msg_y, msg_w, msg_h);
	font_draw(vismsg, msg_x+8, msg_y+8, drawcharacter);
	
	if (DrawCB)
	{
		(*DrawCB)();
	}
}

// "run" messages, i.e. advance # of letters visible as time goes on
// and handle keypresses etc. this code runs at LOGIC framerate
void message_do(void)
{
char btn_down = 0;
	if (!vismsg) return;

	if (DoCB)
	{
		(*DoCB)();
	}

	if (!Menu_IsVisible())		//temphack--should use priority system instead
	{
		if ((keytable[KENTER] && !last_keytable[KENTER]) || \
			(keytable[KSPACE] && !last_keytable[KSPACE]))
		{
			if (typewriter_mode)
			{
				if (showlen != maxlen)
				{
					showlen = maxlen;
					strcpy(vismsg, msg);
				}
				else aftertimer = AFTER_DELAY+1;
			}
			else message_dismiss();
			return;
		}
	}

	if (typewriter_mode)
	{
		if (showlen==maxlen)
		{
			if (!aftertimer && DoneCB) (*DoneCB)();
			if (++aftertimer >= AFTER_DELAY) message_dismiss();				
			return;
		}
		else if (msg_timer > TYPEWRITER_SPEED)
		{
			vismsg[showlen] = msg[showlen];
			showlen++;
			msg_timer = 0;
		}
		else msg_timer++;
	}
}

// returns true if all letters of the message are visible and the
// message timeout has expired
char message_done(void)
{
	return (showlen==maxlen && aftertimer >= AFTER_DELAY);
}

// returns 1 if a message is currently onscreen
char message_up(void)
{
	if (vismsg) return 1; else return 0;
}

// returns 1 if a message is currently onscreen, and the pauses attribute
// was set on it when it was put up
char message_up_and_pausing(void)
{
	return pausing;
}
