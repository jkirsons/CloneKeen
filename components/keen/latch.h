typedef struct EgaHead {
   long LatchPlaneSize;                //Size of one plane of latch data
   long SpritePlaneSize;               //Size of one plane of sprite data
   long OffBitmapTable;                //Offset in EGAHEAD to bitmap table
   long OffSpriteTable;                //Offset in EGAHEAD to sprite table
   short Num8Tiles;                    //Number of 8x8 tiles
   long Off8Tiles;                     //Offset of 8x8 tiles (relative to plane data)
   short Num32Tiles;                   //Number of 32x32 tiles (always 0)
   long Off32Tiles;                    //Offset of 32x32 tiles (relative to plane data)
   short Num16Tiles;                   //Number of 16x16 tiles
   long Off16Tiles;                    //Offset of 16x16 tiles (relative to plane data)
   short NumBitmaps;                   //Number of bitmaps in table
   long OffBitmaps;                    //Offset of bitmaps (relative to plane data)
   short NumSprites;                   //Number of sprites
   long OffSprites;                    //Offset of sprites (relative to plane data)
   short Compressed;                   //(Keen 1 only) Nonzero: LZ compressed data
} EgaHead;

typedef struct BitmapHead {
   short Width;                        // width of bitmap
   short Height;                       // height of bitmap
   long Offset;                        // offset from start of bitmap data
   char Name[8];                       // bitmap name
} BitmapHead;

typedef struct SpriteHead {
   short Width;                        // width of sprite
   short Height;                       // height of sprite
   short OffsetDelta;                  // i don't know what this is
   short OffsetParas;                  // don't know about this either
   short Rx1, Ry1, Rx2, Ry2;           // bounding box for hit detection
   char Name[16];                      // sprite name
} SpriteHead;
