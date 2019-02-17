CFLAGS=-c -g -DTARGET_WIN32   `sdl-config --cflags`

all: tileattr.exe

clean:
	rm -f tileattr.o
	rm -f ../sanity.o
	rm -f ../game.o
	rm -f ../gamedo.o
	rm -f ../gamepdo.o
	rm -f ../gamepdo_wm.o
	rm -f ../editor/editor.o
	rm -f ../console.o
	rm -f ../fileio.o
	rm -f ../maploader.o
	rm -f ../map.o
	rm -f ../graphics.o
	rm -f ../palette.o
	rm -f ../fonts.o
	rm -f ../misc.o
	rm -f ../misc_ui.o
	rm -f ../graphicmaker.o
	rm -f ../ini.o
	rm -f ../intro.o
	rm -f ../menumanager.o
	rm -f ../customepisode.o
	rm -f ../savegame.o
	rm -f ../twirly.o
	rm -f ../sgrle.o
	rm -f ../lprintf.o
	rm -f ../vgatiles.o
	rm -f ../latch.o
	rm -f ../lz.o
	rm -f ../platform.o
	rm -f ../message.o
	rm -f ../cinematics/seqcommon.o
	rm -f ../cinematics/e1ending.o
	rm -f ../cinematics/e3ending.o
	rm -f ../cinematics/blowupworld.o
	rm -f ../cinematics/mortimer.o
	rm -f ../cinematics/TBC.o
	rm -f ../FinaleScreenLoader.o
	rm -f ../globals.o
	rm -f ../ai/yorp.o
	rm -f ../ai/garg.o
	rm -f ../ai/vort.o
	rm -f ../ai/butler.o
	rm -f ../ai/tank.o
	rm -f ../ai/door.o
	rm -f ../ai/ray.o
	rm -f ../ai/icecannon.o
	rm -f ../ai/teleport.o
	rm -f ../ai/rope.o
	rm -f ../ai/walker.o
	rm -f ../ai/tankep2.o
	rm -f ../ai/platform.o
	rm -f ../ai/platvert.o
	rm -f ../ai/vortelite.o
	rm -f ../ai/se.o
	rm -f ../ai/baby.o
	rm -f ../ai/earth.o
	rm -f ../ai/foob.o
	rm -f ../ai/ninja.o
	rm -f ../ai/meep.o
	rm -f ../ai/sndwave.o
	rm -f ../ai/mother.o
	rm -f ../ai/fireball.o
	rm -f ../ai/balljack.o
	rm -f ../ai/nessie.o
	rm -f ../ai/autoray.o
	rm -f ../ai/gotpoints.o
	rm -f ../sdl/keydrv.o
	rm -f ../sdl/snddrv.o
	rm -f ../sdl/timedrv.o
	rm -f ../sdl/viddrv.o
	rm -f ../scale2x/scalebit.o
	rm -f ../scale2x/scale2x.o
	rm -f ../scale2x/scale3x.o
	rm -f ../scale2x/pixel.o

cleanfdh:
	rm -f tileattr.fdh
	rm -f ../sanity.fdh
	rm -f ../game.fdh
	rm -f ../gamedo.fdh
	rm -f ../gamepdo.fdh
	rm -f ../gamepdo_wm.fdh
	rm -f ../editor/editor.fdh
	rm -f ../console.fdh
	rm -f ../fileio.fdh
	rm -f ../maploader.fdh
	rm -f ../map.fdh
	rm -f ../graphics.fdh
	rm -f ../palette.fdh
	rm -f ../fonts.fdh
	rm -f ../misc.fdh
	rm -f ../misc_ui.fdh
	rm -f ../graphicmaker.fdh
	rm -f ../ini.fdh
	rm -f ../intro.fdh
	rm -f ../menumanager.fdh
	rm -f ../customepisode.fdh
	rm -f ../savegame.fdh
	rm -f ../twirly.fdh
	rm -f ../sgrle.fdh
	rm -f ../lprintf.fdh
	rm -f ../vgatiles.fdh
	rm -f ../latch.fdh
	rm -f ../lz.fdh
	rm -f ../platform.fdh
	rm -f ../message.fdh
	rm -f ../cinematics/seqcommon.fdh
	rm -f ../cinematics/e1ending.fdh
	rm -f ../cinematics/e3ending.fdh
	rm -f ../cinematics/blowupworld.fdh
	rm -f ../cinematics/mortimer.fdh
	rm -f ../cinematics/TBC.fdh
	rm -f ../FinaleScreenLoader.fdh
	rm -f ../globals.fdh
	rm -f ../ai/yorp.fdh
	rm -f ../ai/garg.fdh
	rm -f ../ai/vort.fdh
	rm -f ../ai/butler.fdh
	rm -f ../ai/tank.fdh
	rm -f ../ai/door.fdh
	rm -f ../ai/ray.fdh
	rm -f ../ai/icecannon.fdh
	rm -f ../ai/teleport.fdh
	rm -f ../ai/rope.fdh
	rm -f ../ai/walker.fdh
	rm -f ../ai/tankep2.fdh
	rm -f ../ai/platform.fdh
	rm -f ../ai/platvert.fdh
	rm -f ../ai/vortelite.fdh
	rm -f ../ai/se.fdh
	rm -f ../ai/baby.fdh
	rm -f ../ai/earth.fdh
	rm -f ../ai/foob.fdh
	rm -f ../ai/ninja.fdh
	rm -f ../ai/meep.fdh
	rm -f ../ai/sndwave.fdh
	rm -f ../ai/mother.fdh
	rm -f ../ai/fireball.fdh
	rm -f ../ai/balljack.fdh
	rm -f ../ai/nessie.fdh
	rm -f ../ai/autoray.fdh
	rm -f ../ai/gotpoints.fdh
	rm -f ../sdl/keydrv.fdh
	rm -f ../sdl/snddrv.fdh
	rm -f ../sdl/timedrv.fdh
	rm -f ../sdl/viddrv.fdh
	rm -f ../scale2x/scalebit.fdh
	rm -f ../scale2x/scale2x.fdh
	rm -f ../scale2x/scale3x.fdh
	rm -f ../scale2x/pixel.fdh

cleanall: clean cleanfdh

tileattr.exe:  tileattr.o ../sanity.o ../game.o ../gamedo.o \
	 ../gamepdo.o ../gamepdo_wm.o ../editor/editor.o ../console.o ../fileio.o \
	 ../maploader.o ../map.o ../graphics.o ../palette.o ../fonts.o \
	 ../misc.o ../misc_ui.o ../graphicmaker.o ../ini.o ../intro.o \
	 ../menumanager.o ../customepisode.o ../savegame.o ../twirly.o ../sgrle.o \
	 ../lprintf.o ../vgatiles.o ../latch.o ../lz.o ../platform.o \
	 ../message.o ../cinematics/seqcommon.o ../cinematics/e1ending.o ../cinematics/e3ending.o ../cinematics/blowupworld.o \
	 ../cinematics/mortimer.o ../cinematics/TBC.o ../FinaleScreenLoader.o ../globals.o ../ai/yorp.o \
	 ../ai/garg.o ../ai/vort.o ../ai/butler.o ../ai/tank.o ../ai/door.o \
	 ../ai/ray.o ../ai/icecannon.o ../ai/teleport.o ../ai/rope.o ../ai/walker.o \
	 ../ai/tankep2.o ../ai/platform.o ../ai/platvert.o ../ai/vortelite.o ../ai/se.o \
	 ../ai/baby.o ../ai/earth.o ../ai/foob.o ../ai/ninja.o ../ai/meep.o \
	 ../ai/sndwave.o ../ai/mother.o ../ai/fireball.o ../ai/balljack.o ../ai/nessie.o \
	 ../ai/autoray.o ../ai/gotpoints.o ../sdl/keydrv.o ../sdl/snddrv.o ../sdl/timedrv.o \
	 ../sdl/viddrv.o ../scale2x/scalebit.o ../scale2x/scale2x.o ../scale2x/scale3x.o ../scale2x/pixel.o
	gcc -o tileattr.exe \
	 tileattr.o ../sanity.o ../game.o ../gamedo.o \
	 ../gamepdo.o ../gamepdo_wm.o ../editor/editor.o ../console.o ../fileio.o \
	 ../maploader.o ../map.o ../graphics.o ../palette.o ../fonts.o \
	 ../misc.o ../misc_ui.o ../graphicmaker.o ../ini.o ../intro.o \
	 ../menumanager.o ../customepisode.o ../savegame.o ../twirly.o ../sgrle.o \
	 ../lprintf.o ../vgatiles.o ../latch.o ../lz.o ../platform.o \
	 ../message.o ../cinematics/seqcommon.o ../cinematics/e1ending.o ../cinematics/e3ending.o ../cinematics/blowupworld.o \
	 ../cinematics/mortimer.o ../cinematics/TBC.o ../FinaleScreenLoader.o ../globals.o ../ai/yorp.o \
	 ../ai/garg.o ../ai/vort.o ../ai/butler.o ../ai/tank.o ../ai/door.o \
	 ../ai/ray.o ../ai/icecannon.o ../ai/teleport.o ../ai/rope.o ../ai/walker.o \
	 ../ai/tankep2.o ../ai/platform.o ../ai/platvert.o ../ai/vortelite.o ../ai/se.o \
	 ../ai/baby.o ../ai/earth.o ../ai/foob.o ../ai/ninja.o ../ai/meep.o \
	 ../ai/sndwave.o ../ai/mother.o ../ai/fireball.o ../ai/balljack.o ../ai/nessie.o \
	 ../ai/autoray.o ../ai/gotpoints.o ../sdl/keydrv.o ../sdl/snddrv.o ../sdl/timedrv.o \
	 ../sdl/viddrv.o ../scale2x/scalebit.o ../scale2x/scale2x.o ../scale2x/scale3x.o ../scale2x/pixel.o \
	 `sdl-config --libs`


tileattr.o:	tileattr.c tileattr.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) tileattr.c -o tileattr.o

../sanity.o:	../sanity.c ../sanity.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../sanity.c -o ../sanity.o

../game.o:	../game.c ../game.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../game.c -o ../game.o

../gamedo.o:	../gamedo.c ../gamedo.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../gamedo.c -o ../gamedo.o

../gamepdo.o:	../gamepdo.c ../gamepdo.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../gamepdo.c -o ../gamepdo.o

../gamepdo_wm.o:	../gamepdo_wm.c ../gamepdo_wm.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../gamepdo_wm.c -o ../gamepdo_wm.o

../editor/editor.o:	../editor/editor.c ../editor/editor.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../editor/editor.c -o ../editor/editor.o

../console.o:	../console.c ../console.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../console.c -o ../console.o

../fileio.o:	../fileio.c ../fileio.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../fileio.c -o ../fileio.o

../maploader.o:	../maploader.c ../maploader.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h ../maploader.h
	gcc $(CFLAGS) ../maploader.c -o ../maploader.o

../map.o:	../map.c ../map.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../map.c -o ../map.o

../graphics.o:	../graphics.c ../graphics.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../graphics.c -o ../graphics.o

../palette.o:	../palette.c ../palette.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../palette.c -o ../palette.o

../fonts.o:	../fonts.c ../fonts.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../fonts.c -o ../fonts.o

../misc.o:	../misc.c ../misc.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../misc.c -o ../misc.o

../misc_ui.o:	../misc_ui.c ../misc_ui.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../misc_ui.c -o ../misc_ui.o

../graphicmaker.o:	../graphicmaker.c ../graphicmaker.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h ../ai/vort.h
	gcc $(CFLAGS) ../graphicmaker.c -o ../graphicmaker.o

../ini.o:	../ini.c ../ini.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ini.c -o ../ini.o

../intro.o:	../intro.c ../intro.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h ../pressf10.h
	gcc $(CFLAGS) ../intro.c -o ../intro.o

../menumanager.o:	../menumanager.c ../menumanager.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h ../menu_options.c ../menu_keysetup.c \
		../menu_savegames.c ../menu_custommap.c ../editor/menu_editor.c
	gcc $(CFLAGS) ../menumanager.c -o ../menumanager.o

../customepisode.o:	../customepisode.c ../customepisode.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../customepisode.c -o ../customepisode.o

../savegame.o:	../savegame.c ../savegame.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../savegame.c -o ../savegame.o

../twirly.o:	../twirly.c ../twirly.fdh
	gcc $(CFLAGS) ../twirly.c -o ../twirly.o

../sgrle.o:	../sgrle.c ../sgrle.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../sgrle.c -o ../sgrle.o

../lprintf.o:	../lprintf.c ../lprintf.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../lprintf.c -o ../lprintf.o

../vgatiles.o:	../vgatiles.c ../vgatiles.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h ../tga.c ../tga.h
	gcc $(CFLAGS) ../vgatiles.c -o ../vgatiles.o

../latch.o:	../latch.c ../latch.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../latch.c -o ../latch.o

../lz.o:	../lz.c ../lz.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../lz.c -o ../lz.o

../platform.o:	../platform.c ../platform.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../platform.c -o ../platform.o

../message.o:	../message.c ../message.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../message.c -o ../message.o

../cinematics/seqcommon.o:	../cinematics/seqcommon.c ../cinematics/seqcommon.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../cinematics/seqcommon.c -o ../cinematics/seqcommon.o

../cinematics/e1ending.o:	../cinematics/e1ending.c ../cinematics/e1ending.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../cinematics/e1ending.c -o ../cinematics/e1ending.o

../cinematics/e3ending.o:	../cinematics/e3ending.c ../cinematics/e3ending.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../cinematics/e3ending.c -o ../cinematics/e3ending.o

../cinematics/blowupworld.o:	../cinematics/blowupworld.c ../cinematics/blowupworld.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../cinematics/blowupworld.c -o ../cinematics/blowupworld.o

../cinematics/mortimer.o:	../cinematics/mortimer.c ../cinematics/mortimer.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../cinematics/mortimer.c -o ../cinematics/mortimer.o

../cinematics/TBC.o:	../cinematics/TBC.c ../cinematics/TBC.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../cinematics/TBC.c -o ../cinematics/TBC.o

../FinaleScreenLoader.o:	../FinaleScreenLoader.c ../FinaleScreenLoader.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../FinaleScreenLoader.c -o ../FinaleScreenLoader.o

../globals.o:	../globals.c ../globals.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../globals.c -o ../globals.o

../ai/yorp.o:	../ai/yorp.c ../ai/yorp.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/yorp.c -o ../ai/yorp.o

../ai/garg.o:	../ai/garg.c ../ai/garg.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/garg.c -o ../ai/garg.o

../ai/vort.o:	../ai/vort.c ../ai/vort.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h ../ai/vort.h
	gcc $(CFLAGS) ../ai/vort.c -o ../ai/vort.o

../ai/butler.o:	../ai/butler.c ../ai/butler.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/butler.c -o ../ai/butler.o

../ai/tank.o:	../ai/tank.c ../ai/tank.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/tank.c -o ../ai/tank.o

../ai/door.o:	../ai/door.c ../ai/door.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/door.c -o ../ai/door.o

../ai/ray.o:	../ai/ray.c ../ai/ray.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h ../ai/ray.h
	gcc $(CFLAGS) ../ai/ray.c -o ../ai/ray.o

../ai/icecannon.o:	../ai/icecannon.c ../ai/icecannon.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/icecannon.c -o ../ai/icecannon.o

../ai/teleport.o:	../ai/teleport.c ../ai/teleport.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/teleport.c -o ../ai/teleport.o

../ai/rope.o:	../ai/rope.c ../ai/rope.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h ../ai/vort.h
	gcc $(CFLAGS) ../ai/rope.c -o ../ai/rope.o

../ai/walker.o:	../ai/walker.c ../ai/walker.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/walker.c -o ../ai/walker.o

../ai/tankep2.o:	../ai/tankep2.c ../ai/tankep2.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/tankep2.c -o ../ai/tankep2.o

../ai/platform.o:	../ai/platform.c ../ai/platform.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/platform.c -o ../ai/platform.o

../ai/platvert.o:	../ai/platvert.c ../ai/platvert.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/platvert.c -o ../ai/platvert.o

../ai/vortelite.o:	../ai/vortelite.c ../ai/vortelite.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/vortelite.c -o ../ai/vortelite.o

../ai/se.o:	../ai/se.c ../ai/se.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h ../ai/ray.h
	gcc $(CFLAGS) ../ai/se.c -o ../ai/se.o

../ai/baby.o:	../ai/baby.c ../ai/baby.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/baby.c -o ../ai/baby.o

../ai/earth.o:	../ai/earth.c ../ai/earth.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/earth.c -o ../ai/earth.o

../ai/foob.o:	../ai/foob.c ../ai/foob.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/foob.c -o ../ai/foob.o

../ai/ninja.o:	../ai/ninja.c ../ai/ninja.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/ninja.c -o ../ai/ninja.o

../ai/meep.o:	../ai/meep.c ../ai/meep.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/meep.c -o ../ai/meep.o

../ai/sndwave.o:	../ai/sndwave.c ../ai/sndwave.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/sndwave.c -o ../ai/sndwave.o

../ai/mother.o:	../ai/mother.c ../ai/mother.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/mother.c -o ../ai/mother.o

../ai/fireball.o:	../ai/fireball.c ../ai/fireball.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h ../ai/ray.h
	gcc $(CFLAGS) ../ai/fireball.c -o ../ai/fireball.o

../ai/balljack.o:	../ai/balljack.c ../ai/balljack.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/balljack.c -o ../ai/balljack.o

../ai/nessie.o:	../ai/nessie.c ../ai/nessie.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/nessie.c -o ../ai/nessie.o

../ai/autoray.o:	../ai/autoray.c ../ai/autoray.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/autoray.c -o ../ai/autoray.o

../ai/gotpoints.o:	../ai/gotpoints.c ../ai/gotpoints.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../ai/gotpoints.c -o ../ai/gotpoints.o

../sdl/keydrv.o:	../sdl/keydrv.c ../sdl/keydrv.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../sdl/keydrv.c -o ../sdl/keydrv.o

../sdl/snddrv.o:	../sdl/snddrv.c ../sdl/snddrv.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h ../sdl/snddrv.h
	gcc $(CFLAGS) ../sdl/snddrv.c -o ../sdl/snddrv.o

../sdl/timedrv.o:	../sdl/timedrv.c ../sdl/timedrv.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../sdl/timedrv.c -o ../sdl/timedrv.o

../sdl/viddrv.o:	../sdl/viddrv.c ../sdl/viddrv.fdh ../keen.h ../osparam.h \
		../sounds.h ../latch.h ../game.h \
		../keys.h ../macros.h ../ai/enemydata.h \
		../globals.h
	gcc $(CFLAGS) ../sdl/viddrv.c -o ../sdl/viddrv.o

../scale2x/scalebit.o:	../scale2x/scalebit.c ../scale2x/scalebit.fdh ../scale2x/scale2x.h ../scale2x/scale3x.h
	gcc $(CFLAGS) ../scale2x/scalebit.c -o ../scale2x/scalebit.o

../scale2x/scale2x.o:	../scale2x/scale2x.c ../scale2x/scale2x.fdh ../scale2x/scale2x.h
	gcc $(CFLAGS) ../scale2x/scale2x.c -o ../scale2x/scale2x.o

../scale2x/scale3x.o:	../scale2x/scale3x.c ../scale2x/scale3x.fdh ../scale2x/scale3x.h
	gcc $(CFLAGS) ../scale2x/scale3x.c -o ../scale2x/scale3x.o

../scale2x/pixel.o:	../scale2x/pixel.c ../scale2x/pixel.fdh ../scale2x/pixel.h
	gcc $(CFLAGS) ../scale2x/pixel.c -o ../scale2x/pixel.o
