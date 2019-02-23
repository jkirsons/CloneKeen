# Installation Instructions

1. Copy Keen.fw to the SD card and place it in the folder /odroid/firmware/
2. Create a folder on the SD card: /data/keen/
3. Download the CloneKeen source code and extract it to a temporary folder:
>http://clonekeen.sourceforge.net/files/clonekeen-src-84.tar.gz
4. Open the extracted CloneKeen source folder and navigate to: /keen/bin/
5. Copy everything from this folder to /data/keen/ on the SD Card.
6. Copy all the files from the Full version or Shareware Commander Keen Episodes 1, 2 & 3 to the SD card folder: /data/keen/data/
7. Put the SD card in the Odroid-Go, then start while holding B.
8. Flash the Keen firmware. 


>Example SD card files:
>/odroid/firmware/Keen.fw
>/data/keen/keen.conf
>/data/keen/palette.ini
>/data/keen/strings.dat
>/data/keen/...
>/data/keen/data/CTLPANEL.CK1
>/data/keen/data/CTLPANEL.CK2
>/data/keen/data/CTLPANEL.CK3
>/data/keen/data/...
>/data/keen/gfx/100.tga
>/data/keen/gfx/200.tga
>/data/keen/gfx/500.tga
>/data/keen/gfx/...

## Troubleshooting

If you have problems starting up, have a look at the log file: /data/keen/ck.log

It may have more information on what your problem is.