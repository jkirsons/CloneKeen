// miscellaneous UI stuff
#include "keen.h"
#include "misc_ui.fdh"

char tempbuf[80];

// pops up a message (used by youseeinyourmind and VorticonElder)
int twirly_timer, twirly_frame, twirl_x=-1, twirl_y=-1;
int twirly_cutrate;
void dispmsgstring(char *msgname, char typewriter)
{
int dlgX, dlgY, dlgW, dlgH;
    dlgX = GetStringAttribute(msgname, "LEFT");
    dlgY = GetStringAttribute(msgname, "TOP");
    dlgW = GetStringAttribute(msgname, "WIDTH");
    dlgH = GetStringAttribute(msgname, "HEIGHT");
	message(getstring(msgname), dlgX, dlgY, dlgW, dlgH, typewriter, 1);

	twirl_x = GetStringAttribute(msgname, "TWIRLX");
	twirl_y = GetStringAttribute(msgname, "TWIRLY");
	if (twirl_x != -1 && twirl_y != -1)
	{
		twirl_x = (twirl_x * 8) + dlgX;
		twirl_y = (twirl_y * 8) + dlgY;
		twirly_init(&twirly_frame, &twirly_timer);
		message_SetDrawCallback(ysiym_DrawTwirly);
		message_SetDoCallback(ysiym_SpinTwirly);
		twirly_cutrate = 0;
	}
}

void ysiym_DrawTwirly(void)
{
	twirly_draw(twirl_x, twirl_y, &twirly_frame);
}

void ysiym_SpinTwirly(void)
{
	if (++twirly_cutrate > 3)
	{
		twirly_spin(&twirly_frame, &twirly_timer);
		twirly_cutrate = 0;
	}
}

// display a "you see in your mind" or "vorticon elder" string
// based upon the current level
void Show_Ysiym(int mpx, int mpy)
{
char msgname[40];
char prefix[40];

	switch(levelcontrol.episode)
	{
		case 1: strcpy(prefix, "YSIYM"); break;
		case 2: strcpy(prefix, "ELDER"); break;
		default:
			crash("Show_Ysiym(): there are no ysiym's in episode %d!\n", levelcontrol.episode);
			return;		
	}

	// first, try searching for a string that's got level AND position info.
	// this allows custom episodes to have more than one "ysiym" (garg statue)
    sprintf(msgname, "%s_%d_%02x%02x", prefix, levelcontrol.curlevel, mpx, mpy);
    if (GetStringAttribute(msgname, "LEFT")==-1)
	{
		// now, try just the level
		sprintf(msgname, "%s_%d", prefix, levelcontrol.curlevel);
		if (GetStringAttribute(msgname, "LEFT")==-1)
		{
			// no string for this level...display the default
			sprintf(msgname, "%s_DEFAULT", prefix);
		}
	}

	dispmsgstring(msgname, 0);
}


void youseeinyourmind(int mpx, int mpy)
{
	// pop up the message box
	Show_Ysiym(mpx, mpy);

	// make the statue head stop glowing
	map_chgtile(mpx, mpy, tiles[map.mapdata[mpx][mpy]].chgtile);
}


void VorticonElder(int mpx, int mpy)
{
	// pop up the message box
	Show_Ysiym(mpx, mpy);

	// make the switch stop glowing
	map_chgtile(mpx, mpy, TILE_ELDERSWITCH_USED);
}


int shipbox_x, shipbox_y;
void YourShipNeedsTheseParts(void)
{
int w, h;
char *msgname = "EP1_SHIP";

	shipbox_x = GetStringAttribute(msgname, "LEFT");
	shipbox_y = GetStringAttribute(msgname, "TOP");
	w = GetStringAttribute(msgname, "WIDTH");
	h = GetStringAttribute(msgname, "HEIGHT");

	message(getstring(msgname), shipbox_x, shipbox_y, w, h, 0, 1);
	message_SetDrawCallback(DrawShipPartsNeeded);
}

void DrawShipPartsNeeded(void)
{
int y = shipbox_y + (3*8);
	if (!player[primaryplayer].inventory.HasJoystick)
		drawtile(shipbox_x + (9*8), y, 448);

	if (!player[primaryplayer].inventory.HasBattery)
		drawtile(shipbox_x + (12*8), y, 449);

	if (!player[primaryplayer].inventory.HasVacuum)
		drawtile(shipbox_x + (15*8), y, 450);

	if (!player[primaryplayer].inventory.HasFuel)
		drawtile(shipbox_x + (18*8), y, 451);
}


void ShipEp3(void)
{
char msgname[20];
uchar pick;
static lastpick = -1;
	
	// display one of four random strings
	do { pick = (rnd()%4)+1; } while(pick == lastpick);
	sprintf(msgname, "EP3_SHIP%d", pick);
	lastpick = pick;
	
	dispmsgstring(msgname, 0);
}



int statX,statY,statW,statH;

// draw the status box (which pops up via SPACE, or ENTER)
void DrawStatusBox(int cp)
{
int x,i,j;
	switch(levelcontrol.episode)
	{
		case 1: inventory_draw_ep1(cp); break;
		case 2: inventory_draw_ep2(cp); break;
		case 3: inventory_draw_ep3(cp); break;
	}
	
	// draw score
	i = player[cp].inventory.score;
	sprintf(tempbuf, "%d", i);
	font_draw(tempbuf, (statX+12-strlen(tempbuf))<<3, (statY+2)<<3, drawcharacter);
  
	// "extra life at"
	i = player[cp].inventory.extralifeat;
	sprintf(tempbuf, "%d", i);
	font_draw(tempbuf, (statX+28-strlen(tempbuf))<<3, (statY+2)<<3, drawcharacter);
  
	// lives
	i = player[cp].inventory.lives;
	x = ((statX+1)<<3)+4;
	if (i>7) i=7;
	for(j=0;j<i;j++)
	{
		drawsprite(x, (statY+4)<<3, playerbaseframes[cp]);
		x += sprites[playerbaseframes[cp]].xsize;
	}
}

void static inventory_draw_ep1(int cp)
{
int t;
	statX = GetStringAttribute("EP1_StatusBox", "LEFT");
	statY = GetStringAttribute("EP1_StatusBox", "TOP");
	statW = GetStringAttribute("EP1_StatusBox", "WIDTH");
	statH = GetStringAttribute("EP1_StatusBox", "HEIGHT");

	dialogbox(statX<<3,statY<<3,statW,statH);
	font_draw(getstring("EP1_StatusBox"), (statX+1)<<3, (statY+1)<<3, drawcharacter); 

	// fill in what we have
	// 321: joystick/battery/vacuum/fuel not gotten
	// 414: raygun, 415, pogo
	// 424: yellow/red/green/blue cards
	// 448: ship parts, gotten
	// pogo
	if (player[cp].inventory.HasPogo) drawtile(((statX+12)<<3)+4, ((statY+9)<<3)+3, 415);
	// cards
	if (player[cp].inventory.HasCardYellow) drawtile((statX+21)<<3, ((statY+8)<<3)+3, 424);
	if (player[cp].inventory.HasCardRed) drawtile((statX+25)<<3, ((statY+8)<<3)+3, 425);
	if (player[cp].inventory.HasCardGreen) drawtile((statX+21)<<3, ((statY+10)<<3)+4, 426);
	if (player[cp].inventory.HasCardBlue) drawtile((statX+25)<<3, ((statY+10)<<3)+4, 427);
	// ship parts
	if (player[cp].inventory.HasJoystick) t=448; else t=321;
	drawtile((statX+18)<<3, ((statY+4)<<3)+3, t);
	if (player[cp].inventory.HasBattery) t=449; else t=322;
	drawtile((statX+21)<<3, ((statY+4)<<3)+3, t);
	if (player[cp].inventory.HasVacuum) t=450; else t=323;
	drawtile((statX+24)<<3, ((statY+4)<<3)+3, t);
	if (player[cp].inventory.HasFuel) t=451; else t=324;
	drawtile((statX+27)<<3, ((statY+4)<<3)+3, t);

	// raygun icon
	drawtile((statX+4)<<3, ((statY+8)<<3)+3, 414);
	
	// ray gun charges
	showcharges_ep13(cp, (statX+4)<<3, (statY+12)<<3);
}

void static inventory_draw_ep2(int cp)
{
int i;
	statX = GetStringAttribute("EP2_StatusBox", "LEFT");
	statY = GetStringAttribute("EP2_StatusBox", "TOP");
	statW = GetStringAttribute("EP2_StatusBox", "WIDTH");
	statH = GetStringAttribute("EP2_StatusBox", "HEIGHT");

	dialogbox(statX<<3,statY<<3,statW,statH);
	font_draw(getstring("EP2_StatusBox"), (statX+1)<<3, (statY+1)<<3, drawcharacter); 

	// cards
	if (player[cp].inventory.HasCardYellow) drawtile(((statX+21)<<3)-4, ((statY+8)<<3)+3, 424);
	if (player[cp].inventory.HasCardRed) drawtile(((statX+25)<<3)-4, ((statY+8)<<3)+3, 425);
	if (player[cp].inventory.HasCardGreen) drawtile(((statX+21)<<3)-4, ((statY+10)<<3)+4, 426);
	if (player[cp].inventory.HasCardBlue) drawtile(((statX+25)<<3)-4, ((statY+10)<<3)+4, 427);
	// cities saved
	if (levelcontrol.levels_completed[4]) font_draw(getstring("EP2_LVL4_TargetName"), (statX+1)<<3, (statY+8)<<3, drawcharacter);
	if (levelcontrol.levels_completed[6]) font_draw(getstring("EP2_LVL6_TargetName"), (statX+8)<<3, (statY+8)<<3, drawcharacter);
	if (levelcontrol.levels_completed[7]) font_draw(getstring("EP2_LVL7_TargetName"), (statX+1)<<3, (statY+9)<<3, drawcharacter);
	if (levelcontrol.levels_completed[11]) font_draw(getstring("EP2_LVL11_TargetName"), (statX+1)<<3, (statY+10)<<3, drawcharacter);
	if (levelcontrol.levels_completed[9]) font_draw(getstring("EP2_LVL9_TargetName"), (statX+8)<<3, (statY+10)<<3, drawcharacter);
	if (levelcontrol.levels_completed[13]) font_draw(getstring("EP2_LVL13_TargetName"), (statX+1)<<3, (statY+11)<<3, drawcharacter);
	if (levelcontrol.levels_completed[15]) font_draw(getstring("EP2_LVL15_TargetName"), (statX+8)<<3, (statY+9)<<3, drawcharacter);
	if (levelcontrol.levels_completed[16]) font_draw(getstring("EP2_LVL16_TargetName"), (statX+8)<<3, (statY+11)<<3, drawcharacter);

	// raygun icon
	drawtile((statX+20)<<3, ((statY+5)<<3)-5, 414);

	// ray gun charges text
	i = player[cp].inventory.charges;
	if (i>999) i = 999;
	sprintf(tempbuf, "%d", i);
	font_draw(tempbuf, (statX+27-strlen(tempbuf))<<3, ((statY+5)<<3)-1, drawcharacter);
}

void static inventory_draw_ep3(int cp)
{
int ankhtimepercent;

  statX = GetStringAttribute("EP3_StatusBox", "LEFT");
  statY = GetStringAttribute("EP3_StatusBox", "TOP");
  statW = GetStringAttribute("EP3_StatusBox", "WIDTH");
  statH = GetStringAttribute("EP3_StatusBox", "HEIGHT");

  dialogbox(statX<<3,statY<<3,statW,statH);
  font_draw(getstring("EP3_StatusBox"), (statX+1)<<3, (statY+1)<<3, drawcharacter);

  // calculate % ankh time left
  ankhtimepercent = (int)((float)player[cp].ankhtime / (PLAY_ANKH_TIME/100));
  // ankh time
  drawtile((statX+4)<<3, ((statY+8)<<3)+3, 214);
  sprintf(tempbuf, "%d", ankhtimepercent);
  font_draw(tempbuf, (statX+8)<<3, ((statY+8)<<3)+7, drawcharacter);

  // raygun icon
  drawtile((statX+23)<<3, ((statY+5)<<3)-5, 216);

  // ray gun charges
  showcharges_ep13(cp, (statX+26)<<3, ((statY+5)<<3)-1);

  // cards
  if (player[cp].inventory.HasCardYellow) drawtile(((statX+14)<<3)+4, ((statY+8)<<3)+4, 217);
  if (player[cp].inventory.HasCardRed) drawtile(((statX+18)<<3)+4, ((statY+8)<<3)+4, 218);
  if (player[cp].inventory.HasCardGreen) drawtile(((statX+22)<<3)+4, ((statY+8)<<3)+4, 219);
  if (player[cp].inventory.HasCardBlue) drawtile(((statX+26)<<3)+4, ((statY+8)<<3)+4, 220);
}


void showcharges_ep13(int cp, int x, int y)
{
int i;
	i = player[cp].inventory.charges;
	if (i > 999)
		strcpy(tempbuf, "999");
	else
		sprintf(tempbuf, "%d", i);
	font_draw(tempbuf, x, y, drawcharacter);
}

