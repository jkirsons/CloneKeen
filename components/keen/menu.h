

#define MAX_MENU_ENTRIES	50
typedef struct
{
	int x, y;						// menu's X and Y coordinates
	
	// menu's default option; if 0xff, cursor is placed at last item, and cannot move.
	// (for about screen)
	int starting_option;
	
	void (*OnEnterFunc)();			// optional "on-entry" function
	void (*OnDismissalFunc)();		// optional "on-exit" function
	// if present, this function is called when user tries to move the cursor past
	// the top or bottom of the menu. if NULL, the cursor wraps.
	void (*ScrollFunc)();
	
	int nEntries;
	struct
	{
		char text[80];				// text of menu item
		int type;					// type of menu item, one of MNU_... defines
		
		// varies meaning depending on "type" above. can be:
		//  * the address of a submenu
		//  * a function to execute when the option is selected
		//  * a function to poll for what text should be drawn for this entry
		//	  (dynamic entries in option screen). in this case "text" is ignored,
		//	  and the function is called (with a different parameter) both every
		//	  time the screen is updated to get text and as notification if the
		//	  user selects the menu item.
		void *ptr;
		int param;					// param to function pointer, if type is MNU_CALLFP
	} entries[MAX_MENU_ENTRIES];
	
} stMenu;


// Menu header definition
#define M_HEADERLEN			7

#define HD_X				0		// menu's X coordinate
#define HD_Y				1		// menu's Y coordinate
#define HD_STARTINGOPT		2		// menu's default option; if 0xFF, cursor is placed at last item, and cannot move
#define HD_ENTERFUNC		3		// funcpointer for when menu is first executed (optional)
#define HD_DISMISSALFUNC	4		// funcpointer for when menu is dismissed (optional)
#define HD_SCROLLFUNC		5		// funcpointer for if user tries to move cursor past top/btm of menu. if NULL, cursor wraps.
#define HD_RESERVED			6		// reserved

// Menu "body" structure
#define M_ENTRYLEN			4

#define ITM_TEXT			0		// text of menu item
#define ITM_TYPE			1		// type of menu item
#define ITM_PTR				2		// pointer to either a function or the menu to switch to
#define ITM_FLAGS			3		// flags, like an argument to the function etc

// Supported ITM_TYPE values:
#define MNU_SEPERATOR		0		// the twirler skips over this option
#define MNU_SWITCHMENU		1		// 3rd arg is address of a submenu
#define MNU_CALLFP			2		// 3rd arg is a funcpointer to call when selected
#define MNU_DYNAMICTEXT		3		// 3rd arg is a funcpointer to get text

// DYNAMICTEXT function prototype:
// char *Prototype(char *MenuText, char set, uchar param)
// MenuText - the original text associated with the menu item
// set - 0 = getting text for item. 1 = menu option was just activated.
// param - argument 4 from the menu array.
// returns: char * pointer to the text to display for the menu item.

#define MAXM		150

// basically, should equal Y distance between options
#define TWIRLY_YOFF_MOVEAMOUNT	8
typedef struct
{
	int curoption;				// currently selected option
	int last_option;			// total number of options in menu
	uchar *(*curmenu)[MAXM];	// pointer to an array of pointers
	int lastkeydir;
	char visible;				// if 0 menu will be hidden
	char cantmove;				// if 1, twirler is stuck on default option

	int curtwirlframe;			// current animation frame of twirly
	int twirltimer;				// twirly animation timer
	int twirly_yoff;			// px amount to vertically offset twirly by
	int twirly_yoff_vector;		// direction (+1, -1, or 0) twirly is moving
} stCurMenu;
extern stCurMenu menu;

extern int Menu_HandleCount;
#include "menumanager.fdh"		// for dismiss_menu(), etc

extern uchar *main_menu[MAXM];
extern uchar *ingame_menu[MAXM];
extern uchar *selectep_menu[MAXM];
extern uchar *custommap_menu[MAXM];
extern uchar *option_menu[MAXM];
extern uchar *keysetup_menu1[MAXM];
extern uchar *keysetup_menu2[MAXM];
extern uchar *about_menu[MAXM];
extern uchar *endgame_menu[MAXM];
extern uchar *loadgame_menu[MAXM];
extern uchar *savegame_menu[MAXM];
extern uchar *editor_menu[MAXM];
extern uchar *edloadmap_menu[MAXM];
extern uchar *edbadlevel_menu[MAXM];
extern uchar *quit_menu[MAXM];
extern uchar *cant_play_ep_menu[MAXM];
extern uchar *cant_play_usermap_menu[MAXM];
