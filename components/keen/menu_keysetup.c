
#include "keen.h"
#include "menu_keysetup.fdh"
#include "menu.h"

void KeyDrv_SetDefaultMappings(void);
uchar tempbuf[80];

uchar *keysetup_menu1[MAXM] = {32,(9*8),  2, keysetup_init, NULL, NULL, NULL,
					"PLAYER 1 SETUP", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,					
					"LEFT         ", MNU_DYNAMICTEXT, KeySetup, KLEFT,
					"RIGHT        ", MNU_DYNAMICTEXT, KeySetup, KRIGHT,
					"UP           ", MNU_DYNAMICTEXT, KeySetup, KUP,
					"DOWN         ", MNU_DYNAMICTEXT, KeySetup, KDOWN,
					"JUMP         ", MNU_DYNAMICTEXT, KeySetup, KCTRL,
					"POGO         ", MNU_DYNAMICTEXT, KeySetup, KALT,
					"FIRE/STATUS  ", MNU_DYNAMICTEXT, KeySetup, KSPACE,
					"STATUS       ", MNU_DYNAMICTEXT, KeySetup, KENTER,
					"", MNU_SEPERATOR, NULL, 0,
					"Save", MNU_CALLFP, SaveKeys, 0,
					"Cancel", MNU_CALLFP, CancelKeys, 0,
					"Defaults", MNU_CALLFP, SetDefaults, 0,
					};
uchar *keysetup_menu2[MAXM] = {32,(9*8),  2, keysetup_init, NULL, NULL, NULL,
					"PLAYER 2 SETUP", MNU_SEPERATOR, NULL, 0,
					"", MNU_SEPERATOR, NULL, 0,					
					"LEFT         ", MNU_DYNAMICTEXT, KeySetup, KLEFT2,
					"RIGHT        ", MNU_DYNAMICTEXT, KeySetup, KRIGHT2,
					"UP           ", MNU_DYNAMICTEXT, KeySetup, KUP2,
					"DOWN         ", MNU_DYNAMICTEXT, KeySetup, KDOWN2,
					"JUMP         ", MNU_DYNAMICTEXT, KeySetup, KCTRL2,
					"POGO         ", MNU_DYNAMICTEXT, KeySetup, KALT2,
					"FIRE/STATUS  ", MNU_DYNAMICTEXT, KeySetup, KSPACE2,
					"STATUS       ", MNU_DYNAMICTEXT, KeySetup, KENTER2,
					"", MNU_SEPERATOR, NULL, 0,
					"Save", MNU_CALLFP, SaveKeys, 0,
					"Cancel", MNU_CALLFP, CancelKeys, 0,
					"Defaults", MNU_CALLFP, SetDefaults, 0,
					};

void keysetup_init(void)
{
	KeyDrv_EnteredKeySetupMenu();
}


char *KeySetup(char *MenuText, char set, uchar key)
{
char *k2;
	
	if (!set)
	{
		strcpy(tempbuf, MenuText);
		strcat(tempbuf, KeyDrv_GetKeyMapping(key, 0));
		k2 = KeyDrv_GetKeyMapping(key, 1);
		if (k2[0])
		{
			while(strlen(tempbuf)<20) strcat(tempbuf, " ");
			strcat(tempbuf, " / ");
			strcat(tempbuf, k2);
		}

		return tempbuf;
	}
	else
	{
		KeyDrv_SetKeyMapping(key);
		return NULL;
	}
}

char SetDefaults(void)
{
	KeyDrv_SetDefaultMappings();
	return 0;
}

char SaveKeys(void)
{
	KeyDrv_SaveKeyMappings();
	return 1;
}

char CancelKeys(void)
{
	KeyDrv_LoadKeyMappings();
	Menu_InitMenu(&main_menu);
	return 0;
}
