idf.py fullclean
idf.py build
/Applications/ffmpeg -i ./images/Tile.png -f rawvideo -pix_fmt rgb565 ./images/tile.raw
~/Documents/GitHub/odroid-go-firmware-20181001/tools/mkfw/mkfw Keen ./images/tile.raw 0 16 1048576 app ./build/CloneKeen.bin
rm ./release/Keen.fw
mv firmware.fw ./release/Keen.fw
