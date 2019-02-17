/* INTRO.C
  The opening intro...
*/

#include "keen.h"
#include "pressf10.h"
#include "intro.fdh"

#define TWIRL_SPEED_SLOW        100
#define TWIRL_SPEED_FAST        20

#define MENUS_Y                 32
#define MAINMENU_X              32

#define FONT_TWIRL              9
#define TWIRL_SPEED             30
#define TWIRL_NUM_FRAMES        5

extern char fade_black;
int intro(void)
{
int x,y,i;
int xb,yb;

int curPage, changedPage;
int textVisible;
char stStringName[40];
char *stStringData;
char stTextLine1[80];
char stTextLine2[80];
char stTextLine3[80];
char SlowPrintBuffer[80];
int x1,x2,x3,x4;
int y1,y2,y3,y4;
char *copyPtr, copyIndex;

int ontime, offtime, textTimer;
int y1adj, y2adj, y3adj;
int lastpage;
int SlowPrint, SlowPrintTimer, SlowPrintIndex;
int SlowPrintSpeed;
char keypressed;
char lastkeypressed;
char *SlowPrintPtr;
                          
char *ScrollText1      = "Presenting";
char *ScrollTextErase  = "               ";
char *ScrollText2      = "CloneKeen";
char *ScrollText4      = "by Caitlin Shaw";
char ScrollText3[20];
char ScrollTextTimer;
int STimer;
char SState;
char ShowPressF10;
#define PRESSF10_X      (160 - (PRESSF10_WIDTH/2))
#define PRESSF10_Y      3
//#define PRESSF10_X      (315 - PRESSF10_WIDTH)
//#define PRESSF10_Y      (195 - PRESSF10_HEIGHT)

  if (options[OPT_FULLSCREEN] && options[OPT_ZOOM]>1)
    ShowPressF10 = 0;           // 0 = mask hide all
  else
    ShowPressF10 = 15;          // 15 = mask show all

  // display the starry background and initiate a fade-in
  showmapatpos(90, 104<<4, 32);
  fade(FADE_IN, FADE_NORM);

  #define SCROLL_STOP_Y          75
  #define F10_STOP_Y             210

  /* move "Presenting" up the screen */
  fade_black = 1;
  sprintf(ScrollText3, "Episode %d", levelcontrol.episode);
  x1 = (WINDOW_WIDTH/2) - ((strlen(ScrollText1)*8)/2);
  y1 = WINDOW_HEIGHT+35;
  x2 = (WINDOW_WIDTH/2) - ((strlen(ScrollText2)*8)/2);
  y2 = SCROLL_STOP_Y+16;
  x3 = (WINDOW_WIDTH/2) - ((strlen(ScrollText3)*8)/2);
  y3 = y2+24;
  x4 = (WINDOW_WIDTH/2) - ((strlen(ScrollText4)*8)/2);
  y4 = y3+16;
  SState = 0;
  STimer = 0;
  ScrollTextTimer = 0;

  lastkeypressed = 1;
  do
  {
    // do fades
    gamedo_fades();
    gamedo_AnimatedTiles();  

    if (fadecontrol.dir != FADE_OUT)
    {
       // if user pressed a key cancel the intro
       keypressed = 0;
       for(i=0;i<KEYTABLE_SIZE;i++)
       {
          if (keytable[i])
          {
             keypressed = i;
             break;
          }
       }
       if (keypressed && !lastkeypressed)
       {
             if (keypressed == KF10)
             {
                if (!options[OPT_ZOOM] || options[OPT_FULLSCREEN] != 1)
                {
                   options[OPT_FULLSCREEN] = 1;
                   options[OPT_ZOOM] = 1;
                   VidDrv_reset();
                   map_redraw();
                }
             }
             else
             {
                fadecontrol.dir = FADE_OUT;
                fadecontrol.curamt = PAL_FADE_SHADES;
                fadecontrol.fadetimer = 0;
                fadecontrol.rate = FADE_NORM;
                fadecontrol.mode = FADE_GO;
             }
       }
       if (keypressed) lastkeypressed = 1; else lastkeypressed = 0;

    }  // end if(fadecontrol.dir!=FADE_OUT)

    ////sb_font_draw_inverse(ScrollText1, x1, y1);

    if (SState >= 1)
    {
      ////sb_font_draw_inverse(ScrollText2, x2, y2);
      if (SState==1)
      {
        if (STimer > 200)
        {
          SState = 2;
          STimer = 0;
        }
        else STimer++;
      }
    }

    if (SState >= 2)
    {
      //sb_font_draw_inverse(ScrollText3, x3, y3);
      if (SState==2)
      {
         if (STimer > 200)
         {
            SState = 3;
            STimer = 0;
         }
         else STimer++;
      }
    }

    if (SState >= 3)
    {
      //sb_font_draw_inverse(ScrollText4, x4, y4);
      if (STimer > 1000)
      {
        fade_black = 0;
        break;
      }
      else STimer++;
    }

    // if Press F10 message is still visible, display it
    if (ShowPressF10)
    {
      if (y1 <= F10_STOP_Y) ShowPressF10 = 0;
      if (options[OPT_ZOOM] && options[OPT_FULLSCREEN]) ShowPressF10 = 0;

      i = 0;
      for(y=0;y<PRESSF10_HEIGHT;y++)
      {
        yb = ((PRESSF10_Y+y+scrolly_buf)&511)<<9;
        for(x=0;x<PRESSF10_WIDTH;x++)
        {
          scrollbuf[yb+((PRESSF10_X+x+scrollx_buf)&511)] = (pressf10_image[i++] & ShowPressF10);
        }
      }
    }

    // blit the scrollbuffer to the display
//    gamedo_frameskipping_blitonly();

    if (SState==0)
    {
       //sb_font_draw_inverse(ScrollTextErase, x1, y1);

       if (y1 > SCROLL_STOP_Y)
       {
          if (ScrollTextTimer > 10)
          {
            y1--;
            ScrollTextTimer = 0;
          }
          else ScrollTextTimer++;
       }
       else
       {
          if (STimer > 200)
          {
            SState = 1;
            STimer = 0;
          }
          else STimer++;
       }
    }

    // when fade is complete we're done!
    if (fadecontrol.dir==FADE_OUT && fadecontrol.mode==FADE_COMPLETE)
    {
      fade_black = 0;
      return 0;
    }

    if (keytable[KESC] || crashflag)
    {
       fade_black = 0;
       return 1;
    }
  } while(1);

  //sb_font_draw_inverse(ScrollTextErase, x1, y1);
  //sb_font_draw_inverse(ScrollTextErase, x2, y2);
  //sb_font_draw_inverse(ScrollTextErase, x3, y3);
  //sb_font_draw_inverse(ScrollTextErase, x4, y4);

  /* the appearing/disapearing text */
  curPage = 1;
  changedPage = 1;
  do
  {
    // do fades
    gamedo_fades();
    gamedo_AnimatedTiles();

       // need to load a new page from the strings file?
       if (changedPage)
       {
          // load the data for the new page
          sprintf(stStringName, "Tribute_Page_%d", curPage);
          stStringData = getstring(stStringName);
          ontime = GetStringAttribute(stStringName, "ONTIME");
          offtime = GetStringAttribute(stStringName, "OFFTIME");
          y1adj = GetStringAttribute(stStringName, "Y1ADJ");
          y2adj = GetStringAttribute(stStringName, "Y2ADJ");
          y3adj = GetStringAttribute(stStringName, "Y3ADJ");
          lastpage = GetStringAttribute(stStringName, "LASTPAGE");
          SlowPrint = GetStringAttribute(stStringName, "SLOWPRINT");

          // if no SLOWPRINT attribute disable slowprinting
          if (SlowPrint==-1)
          {
            SlowPrint = 0;
          }
          else
          {
            SlowPrintSpeed = GetStringAttribute(stStringName, "SLOWPRINTSPD");
          }

          // for y adjustments that weren't specified use 0
          if (y1adj==-1) y1adj = 0;
          if (y2adj==-1) y2adj = 0;
          if (y3adj==-1) y3adj = 0;

          stTextLine1[0] = stTextLine2[0] = stTextLine3[0] = 0;

          // we have the text as three CR-terminated lines, now split it
          // up into the 3 buffers
          copyIndex = 0;
          copyPtr = stTextLine1;
          for(i=0;i<strlen(stStringData);i++)
          {
            if (stStringData[i] != 13)
            {
              *copyPtr = stStringData[i];
              copyPtr++;
            }
            else
            {  // hit a CR
              // null-terminate
              *copyPtr = 0;
              // start copying to next buffer
              if (copyIndex==0) copyPtr = stTextLine2;
              else if (copyIndex==1) copyPtr = stTextLine3;
              else if (copyIndex==2) break;
              copyIndex++;
            }
          }
          *copyPtr = 0;    // null-terminate

          // figure out what X position these lines to be drawn in order
          // to be centered.
          x1 = (WINDOW_WIDTH/2)-((strlen(stTextLine1)*8)/2);
          x2 = (WINDOW_WIDTH/2)-((strlen(stTextLine2)*8)/2);
          x3 = (WINDOW_WIDTH/2)-((strlen(stTextLine3)*8)/2);
          // figure out their Y positions
          y1 = 90 + y1adj;
          y2 = 98 + y2adj;
          y3 = 106 + y3adj;

          // if we're going to do slowprinting then copy the line we're going
          // to slowprint into the slowprint buffer and fill it with spaces
          if (SlowPrint)
          {
            // set up a pointer to the line we're going to slowprint
            if (SlowPrint==1) SlowPrintPtr = &stTextLine1[0];
            else if (SlowPrint==2) SlowPrintPtr = &stTextLine2[0];
            else SlowPrintPtr = &stTextLine3[0];
   
            // copy the text line into the slow print buffer.
            // replace '@''s with the episode number
            for(i=0;i<strlen(SlowPrintBuffer)+1;i++)
            {
              if (SlowPrintPtr[i]=='@')
              {
                SlowPrintBuffer[i] = levelcontrol.episode + '0';
              }
              else
              {
                SlowPrintBuffer[i] = SlowPrintPtr[i];
              }
            }

            // clear out the text line
            for(i=0;i<strlen(SlowPrintPtr);i++) SlowPrintPtr[i] = ' ';
          }

          // set up some variables
          textVisible = 1;
          textTimer = ontime;
          changedPage = 0;
          SlowPrintTimer = 0;
          SlowPrintIndex = 0;
       }

       // handle slowprinting
       if (SlowPrint)
       {
          if (SlowPrintTimer > SlowPrintSpeed)
          {  // time to print the next character of the line we're slowprinting
             if (SlowPrintBuffer[SlowPrintIndex])
             {
               SlowPrintPtr[SlowPrintIndex] = SlowPrintBuffer[SlowPrintIndex];
               SlowPrintIndex++;
             }
             else
             { // reached the NULL (slowprint complete)
               SlowPrint = 0;
             }
   
             SlowPrintTimer = 0;
          }
          else SlowPrintTimer++;
       }

    if (fadecontrol.dir != FADE_OUT)
    {
       if (!textTimer)
       {  // time to either invisibilize the text, or go to the next page
         if (textVisible)
         {
           // erase the text
           for(i=0;i<strlen(stTextLine1);i++) stTextLine1[i] = ' ';
           for(i=0;i<strlen(stTextLine2);i++) stTextLine2[i] = ' ';
           for(i=0;i<strlen(stTextLine3);i++) stTextLine3[i] = ' ';
           textVisible = 0;
           textTimer = offtime;
         }
         else
         {
           // time for more text
           if (lastpage != 1)
           {
             curPage++;
             changedPage = 1;
           }
           else
           {  // reached last page...initiate fadeout
             fadecontrol.dir = FADE_OUT;
             fadecontrol.curamt = PAL_FADE_SHADES;
             fadecontrol.fadetimer = 0;
             fadecontrol.rate = FADE_NORM;
             fadecontrol.mode = FADE_GO;
           }
         }
       }
       else textTimer--;

       // if user pressed a key cancel the intro
       keypressed = 0;
       for(i=0;i<KEYTABLE_SIZE;i++)
       {
          if (keytable[i])
          {
             keypressed = 1;
             break;
          }
       }
       if (keypressed && !lastkeypressed)
       {
             fadecontrol.dir = FADE_OUT;
             fadecontrol.curamt = PAL_FADE_SHADES;
             fadecontrol.fadetimer = 0;
             fadecontrol.rate = FADE_NORM;
             fadecontrol.mode = FADE_GO;
       }
       lastkeypressed = keypressed;


    }  // end if(fadecontrol.dir!=FADE_OUT)
  
    // draw/erase all three lines of text
    //sb_font_draw_inverse(stTextLine1, x1, y1);
    //sb_font_draw_inverse(stTextLine2, x2, y2);
    //sb_font_draw_inverse(stTextLine3, x3, y3);

    // blit the scrollbuffer to the display
    //gamedo_frameskipping_blitonly();

    // when fade is complete we're done!
    if (fadecontrol.dir==FADE_OUT && fadecontrol.mode==FADE_COMPLETE)
    {
      return 0;
    }

  } while(!keytable[KESC] && !crashflag);
  return 1;
}

