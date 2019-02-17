#include "keen.h"

// position where "current enemy" is shown
#define SPR_LEFT			(WINDOW_WIDTH+4)
#define SPR_TOP				20
#define SPRAREA_W			32
#define SPRAREA_H			40
#define NEXTBUTTON_X		(SPR_LEFT-1)
#define NEXTBUTTON_Y		(SPR_TOP+SPRAREA_H+2)
#define LASTBUTTON_X		NEXTBUTTON_X
#define LASTBUTTON_Y		(NEXTBUTTON_Y+BUTTON_H+2)
/* located in fileio.c */

//------------------[referenced from editor/editor.c]----------------//
void SaveOptions(void);

/* located in game.c */

//------------------[referenced from editor/editor.c]----------------//
uchar spawn_object(int x, int y, int otype);
void delete_object(int o);
void game_SetStartPositions(void);
void take_all_keycards(void);
void initplayers(void);
void scroll_to_player(void);
void makedark(int dark);
void reviveplayer(int theplayer);
void GiveAnkh(int cp);
void endlevel(int reason_for_leaving);
void initgame(void);

// saves the state objects were in, while the game is paused
extern stObject saved_objects[MAX_OBJECTS];
extern char objects_dirty;

/* located in fonts.c */

//----------------------[referenced from fonts.c]--------------------//
void font_draw(unsigned char *text, int xs, int y, void (*drawingfunc)());
void drawcharacter_clear(int xoff, int yoff, int f);
void drawcharacter_clear_inversepink(int xoff, int yoff, int f);
void drawcharacter_clear_erasebg(int xoff, int yoff, int f);
void drawcharacter(int xoff, int yoff, int f);

#define BUTTON_W			34
#define BUTTON_H			12

// spawn "ghosts" of the objects which are dirty...this appears when the game
// is paused and shows where all the objects will return to when the game
// is run again
void spawn_ghosted_objects(void)
{
int i,o;
	for(i=0;i<MAX_OBJECTS;i++)
	{
		if (saved_objects[i].exists && saved_objects[i].type != OBJ_PLAYER)
		{
			o = spawn_object(saved_objects[i].x, saved_objects[i].y, OBJ_GHOST);
			objects[o].sprite = saved_objects[i].sprite;
		}
	}
}

// sets whether or not to show object "ghosts" when paused
void editor_set_do_ghosting(int newval)
{
int i;
char *ptr;

	options[OPT_GHOSTING] = newval;
	if (objects_dirty)
	{
		if (options[OPT_GHOSTING])
		{
			spawn_ghosted_objects();
		}
		else
		{
			for(i=0;i<MAX_OBJECTS;i++)
			{
				if (objects[i].exists && objects[i].type==OBJ_GHOST)
				{
					delete_object(i);
				}
			}
		}
	}
	
	SaveOptions();
	
	ptr = options[OPT_GHOSTING] ? "GHOST":"     ";
	font_draw(ptr,LASTBUTTON_X-2,LASTBUTTON_Y+BUTTON_H+6+8,drawcharacter_clear_erasebg);
}
