// various macros

// draw at sprite at 1X.
#define drawsprite1x(xoff, yoff, snum) { drawsprite1x_mask(xoff, yoff, snum, sprites[snum].maskdata); }
// draw at sprite at 2X.
#define drawsprite2x(xoff, yoff, snum) { drawsprite2x_mask(xoff, yoff, snum, sprites[snum].maskdata); }
