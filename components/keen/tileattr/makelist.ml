CFLAGS=-c -g -DTARGET_WIN32   `sdl-config --cflags`
LFLAGS=`sdl-config --libs`
COMPILER=gcc
LINKER=gcc
OUTPUT=tileattr.exe

tileattr.c
../engine.ml
#
