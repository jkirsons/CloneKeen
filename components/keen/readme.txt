*************************************
*                                   *
*        C L O N E K E E N          *
*                                   *
*    (c)2003-2005  Caitlin Shaw     *
*      Released under GNU/GPL       *
*                                   *
*             Beta 8.3              *
*                                   *
*************************************
Welcome to CloneKeen!

CloneKeen is an almost almost complete open-source clone of
the old classic DOS game, Commander Keen: Invasion of the
Vorticons by ID Software. CloneKeen requires the original game
data files to work. You can play all three episodes of the game.

CloneKeen currently runs under win32, with SDL, and
under Linux/X11, with SDL.

Drivers for compiling a DOS version that blits using
assembly language are also included, but may need a
little tweaking to get them to compile happy with
the rest of the code as the game was switched from
DOS to SDL in early beta and the drivers have not
been used since. My hope is for CloneKeen to be
ported to many other platforms.

CloneKeen tries to mostly replicate the original game,
however there are also a number of improvements,
including smoother scrolling, better graphic effects
in some parts of the game, and 2-player support.

To my knowledge this is the only released attempt at
writing a third-party Keen game that has reached this
stage of completedness.

The CloneKeen source may be freely distributed and
modified as per the GPL, just please remember to give
credit to the original author, Caitlin Shaw.

CloneKeen comes with one new level for episode 1 (shareware),
and 2 new levels for episode 2. These levels are released
under the same license as the rest of the program.

keen 17 -ep1 -dbl
keen 18 -ep2 -dbl
keen 19 -ep2 -dbl

CloneKeen also includes the Tile Attribute Editor
used while making the game which allows modification
of the level tile properties.


Setting up the binary release of the game:

 * Copy all of the data files from the original game(s) into the
   "keen/bin/data" folder. This means all of the files with the extension
   .CK?. If you have the registered versions, you can play the whole game,
   otherwise download the shareware version and copy the data from it.
 * Go into the "bin" folder and drop to the command line.
 * Execute "keen -ep1 -fs" for episode 1, "keen -ep2 -fs" for episode 2,
   etc. The -fs specifies that CloneKeen should run in fullscreen mode.
 * To double-scale the graphics using the scale2x filter, use the -dbl switch.
 * For full usage instructions run "keen -?".
 * Under win32, debug messages are output to ck.log. 
   If anything goes wrong during startup be sure to check it.

NOTE: The Linux/UNIX release does not include a binary executable.
      You will need the Win32 binary package AND the source distro
      in order to compile and run CloneKeen under UNIX.


Compiling the game on win32:

 * Copy makefile.w32 to makefile
 * You will need mingw and msys.
 * Open up the msys window, go to your src folder and run make.
 * From DOS, run g.bat in the src folder. This will copy the produced
   exe to the bin folder and run it.
 * You must have the binary distro as well in order to use the produced exe.

Compiling the game on Linux/UNIX:

 * Copy makefile.lnx to makefile
 * Chmod the 'g' script so it's executable
 * Run make
 * Issue ./g, this will copy the produced executable to the bin directory
   and run it.
 * You must have the binary distro as well in order to use the produced exe.

For all compilation platforms you will need to already have SDL-devel
installed and working.


HINTS & SECRETS


- CHEAT CODES -

There are a full assortment of cheat codes available. Some cheats must first
be enabled with -cheat or through the options menu. C+T+SPACE and G+O+D will
always work.

KEY     ACTION              DESCRIPTION
G+O+D   God                 Toggles GOD mode. You can't protect yourself
                            from dying if you fall off the map though.
                            (Use TAB to revive in this case).

TAB     Noclip/Revive       Turns off clipping, allowing you to walk through
                            walls / Aborts a dying sequence.

C+T+SPACE  All Items        Gives all items, similar to the original.

F2      Nofall              Disables gravity, so Keen can walk on air.                           

F6      Debug mode/RADAR    Press once: enables debug mode, press again,
                            enables radar in upper-left corner.

F7      Throttle off        Disables speed throttle so the game goes really
                            fast.

F8      Frame-by-frame mode Just as it sounds, see in-game instructions.

F9      Abort level         Marks level as "done", and returns to the map.

Keypad +  Force fall        Allows fall to a platform below the current one.
                            The opposite of F2.

- SAVE/LOAD GAME -

The game be saved by pressing F3 at any point during the game. The game will
be saved in the exact position it was left (unlike the original which only
allowed save at the map).

- DEMO -

Wait a while at the tile screen and the game will cycle through several
demos, an added feature not found on the original game.
                            
****************************
For questions, comments, or updates to the source tree
please contact: rogueeve@mailshack.com or go thru
SourceForge at clonekeen.sourceforge.net
****************************
