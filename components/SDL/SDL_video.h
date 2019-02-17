#ifndef SDL_TFT_H
#define SDL_TFT_H

#include "SDL_stdinc.h"
#include "SDL_error.h"
#include "SDL_rwops.h"
#include "SDL_blit.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include <errno.h>

#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include "spi_lcd.h"

#define SDL_SaveBMP(surface, file) {}

extern DECLSPEC SDL_Surface * SDLCALL SDL_LoadBMP_RW(SDL_RWops *src, int freesrc);

/** Convenience macro -- load a surface from a file */
#define SDL_LoadBMP(file)	SDL_LoadBMP_RW(SDL_RWFromFile(file, "rb"), 1)


typedef struct{
  Uint8 r;
  Uint8 g;
  Uint8 b;
  Uint8 unused;
} SDL_Color;

typedef struct{
  int ncolors;
  SDL_Color *colors;
} SDL_Palette;

typedef struct SDL_PixelFormat {
  SDL_Palette *palette;
  Uint8  BitsPerPixel;
  Uint8  BytesPerPixel;
  Uint8  Rloss, Gloss, Bloss, Aloss;
  Uint8  Rshift, Gshift, Bshift, Ashift;
  Uint32 Rmask, Gmask, Bmask, Amask;
  Uint32 colorkey;
  Uint8  alpha;
} SDL_PixelFormat;

typedef struct SDL_Surface {
        Uint32 flags;                           /* Read-only */
        SDL_PixelFormat *format;                /* Read-only */
        int w, h;                               /* Read-only */
        Uint16 pitch;                           /* Read-only */
        void *pixels;                           /* Read-write */

        int offset;				/**< Private */

        /** Hardware-specific surface info */
        struct private_hwdata *hwdata;

        /** clipping information */
        SDL_Rect clip_rect;			/**< Read-only */
        Uint32 unused1;				/**< for binary compatibility */

        /** Allow recursive locks */
        Uint32 locked;				/**< Private */

        /** info for fast blit mapping to other surfaces */
        struct SDL_BlitMap *map;		/**< Private */

        /** format version, bumped at every change to invalidate blit maps */
        unsigned int format_version;		/**< Private */

        /* Reference count -- used when freeing surface */
        int refcount;                           /* Read-mostly */

	/* This structure also contains private fields not shown here */
} SDL_Surface;

/* These are the currently supported flags for the SDL_surface */
/* Available for SDL_CreateRGBSurface() or SDL_SetVideoMode() */
#define SDL_SWSURFACE	0x00000000	/* Surface is in system memory */
#define SDL_HWSURFACE	0x00000001	/* Surface is in video memory */
#define SDL_ASYNCBLIT	0x00000004	/* Use asynchronous blits if possible */
/* Available for SDL_SetVideoMode() */
#define SDL_ANYFORMAT	0x10000000	/* Allow any video depth/pixel-format */
#define SDL_HWPALETTE	0x20000000	/* Surface has exclusive palette */
#define SDL_DOUBLEBUF	0x40000000	/* Set up double-buffered video mode */
#define SDL_FULLSCREEN	0x80000000	/* Surface is a full screen display */
#define SDL_OPENGL      0x00000002      /* Create an OpenGL rendering context */
#define SDL_OPENGLBLIT	0x0000000A	/* Create an OpenGL rendering context and use it for blitting */
#define SDL_RESIZABLE	0x00000010	/* This video mode may be resized */
#define SDL_NOFRAME	0x00000020	/* No window caption or edge frame */
/* Used internally (read-only) */
#define SDL_HWACCEL	0x00000100	/* Blit uses hardware acceleration */
#define SDL_SRCCOLORKEY	0x00001000	/* Blit uses a source color key */
#define SDL_RLEACCELOK	0x00002000	/* Private flag */
#define SDL_RLEACCEL	0x00004000	/* Surface is RLE encoded */
#define SDL_SRCALPHA	0x00010000	/* Blit uses source alpha blending */
#define SDL_PREALLOC	0x01000000	/* Surface uses preallocated memory */

/* Evaluates to true if the surface needs to be locked before access */
#define SDL_MUSTLOCK(S) (((S)->flags & SDL_RLEACCEL) != 0)

/** @name flags for SDL_SetPalette() */
/*@{*/
#define SDL_LOGPAL 0x01
#define SDL_PHYSPAL 0x02

/** @name Transparency definitions
 *  These define alpha as the opacity of a surface
 */
/*@{*/
#define SDL_ALPHA_OPAQUE 255
#define SDL_ALPHA_TRANSPARENT 0
/*@}*/

/** Useful for determining the video hardware capabilities */
typedef struct SDL_VideoInfo {
	Uint32 hw_available :1;	/**< Flag: Can you create hardware surfaces? */
	Uint32 wm_available :1;	/**< Flag: Can you talk to a window manager? */
	Uint32 UnusedBits1  :6;
	Uint32 UnusedBits2  :1;
	Uint32 blit_hw      :1;	/**< Flag: Accelerated blits HW --> HW */
	Uint32 blit_hw_CC   :1;	/**< Flag: Accelerated blits with Colorkey */
	Uint32 blit_hw_A    :1;	/**< Flag: Accelerated blits with Alpha */
	Uint32 blit_sw      :1;	/**< Flag: Accelerated blits SW --> HW */
	Uint32 blit_sw_CC   :1;	/**< Flag: Accelerated blits with Colorkey */
	Uint32 blit_sw_A    :1;	/**< Flag: Accelerated blits with Alpha */
	Uint32 blit_fill    :1;	/**< Flag: Accelerated color fill */
	Uint32 UnusedBits3  :16;
	Uint32 video_mem;	/**< The total amount of video memory (in K) */
	SDL_PixelFormat *vfmt;	/**< Value: The format of the video surface */
	int    current_w;	/**< Value: The current video mode width */
	int    current_h;	/**< Value: The current video mode height */
} SDL_VideoInfo;

/** typedef for private surface blitting functions */
typedef int (*SDL_blit)(struct SDL_Surface *src, SDL_Rect *srcrect,
			struct SDL_Surface *dst, SDL_Rect *dstrect);

typedef int SDLKey;

void SDL_WM_SetCaption(const char *title, const char *icon);
Uint32 SDL_WasInit(Uint32 flags);
int SDL_InitSubSystem(Uint32 flags);
int SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color);
SDL_Surface *SDL_CreateRGBSurface(Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);

SDL_Surface * SDL_CreateRGBSurfaceFrom (void *pixels,
			int width, int height, int depth, int pitch,
			Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
      
char *SDL_GetKeyName(SDLKey key);
SDL_Surface *SDL_GetVideoSurface(void);
Uint32 SDL_MapRGB(const SDL_PixelFormat * const format, const Uint8 r, const Uint8 g, const Uint8 b);
int SDL_SetColors(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors);
SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags);
void SDL_FreeSurface(SDL_Surface *surface);
void SDL_QuitSubSystem(Uint32 flags);
int SDL_Flip(SDL_Surface *screen);
int SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags);
int SDL_LockSurface(SDL_Surface *surface);
void SDL_UnlockSurface(SDL_Surface* surface);
void SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h);
SDL_Rect **SDL_ListModes(SDL_PixelFormat *format, Uint32 flags);
SDL_VideoInfo *SDL_GetVideoInfo(void);
char *SDL_VideoDriverName(char *namebuf, int maxlen);

/*
 * You should call SDL_BlitSurface() unless you know exactly how SDL
 * blitting works internally and how to use the other blit functions.
 */
#define SDL_BlitSurface SDL_UpperBlit

/** This is the public blit function, SDL_BlitSurface(), and it performs
 *  rectangle validation and clipping before passing it to SDL_LowerBlit()
 */
extern DECLSPEC int SDLCALL SDL_UpperBlit
			(SDL_Surface *src, SDL_Rect *srcrect,
			 SDL_Surface *dst, SDL_Rect *dstrect);
/** This is a semi-private blit function and it performs low-level surface
 *  blitting only.
 */
extern DECLSPEC int SDLCALL SDL_LowerBlit
			(SDL_Surface *src, SDL_Rect *srcrect,
			 SDL_Surface *dst, SDL_Rect *dstrect);

/**
 * Sets a portion of the colormap for a given 8-bit surface.
 * 'flags' is one or both of:
 * SDL_LOGPAL  -- set logical palette, which controls how blits are mapped
 *                to/from the surface,
 * SDL_PHYSPAL -- set physical palette, which controls how pixels look on
 *                the screen
 * Only screens have physical palettes. Separate change of physical/logical
 * palettes is only possible if the screen has SDL_HWPALETTE set.
 *
 * The return value is 1 if all colours could be set as requested, and 0
 * otherwise.
 *
 * SDL_SetColors() is equivalent to calling this function with
 *     flags = (SDL_LOGPAL|SDL_PHYSPAL).
 */
extern DECLSPEC int SDLCALL SDL_SetPalette(SDL_Surface *surface, int flags,
				   SDL_Color *colors, int firstcolor,
				   int ncolors);


extern SemaphoreHandle_t display_mutex;
void SDL_LockDisplay();
void SDL_UnlockDisplay();

typedef unsigned char  JE_byte;
//extern JE_byte ** allocateTwoDimenArrayOnHeapUsingMalloc(int row, int col);


/**
 * Maps a pixel value into the RGB components for a given pixel format
 */
extern DECLSPEC void SDLCALL SDL_GetRGB(Uint32 pixel,
				const SDL_PixelFormat * const fmt,
				Uint8 *r, Uint8 *g, Uint8 *b);

/**
 * Maps a pixel value into the RGBA components for a given pixel format
 */
extern DECLSPEC void SDLCALL SDL_GetRGBA(Uint32 pixel,
				const SDL_PixelFormat * const fmt,
				Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a);

/**
 * Maps an RGBA quadruple to a pixel value for a given pixel format
 */
extern DECLSPEC Uint32 SDLCALL SDL_MapRGBA
(const SDL_PixelFormat * const format,
 const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a);


/**
 * Sets the color key (transparent pixel) in a blittable surface.
 * If 'flag' is SDL_SRCCOLORKEY (optionally OR'd with SDL_RLEACCEL), 
 * 'key' will be the transparent pixel in the source image of a blit.
 * SDL_RLEACCEL requests RLE acceleration for the surface if present,
 * and removes RLE acceleration if absent.
 * If 'flag' is 0, this function clears any current color key.
 * This function returns 0, or -1 if there was an error.
 */
extern DECLSPEC int SDLCALL SDL_SetColorKey
			(SDL_Surface *surface, Uint32 flag, Uint32 key);

/**
 * Creates a new surface of the specified format, and then copies and maps 
 * the given surface to it so the blit of the converted surface will be as 
 * fast as possible.  If this function fails, it returns NULL.
 *
 * The 'flags' parameter is passed to SDL_CreateRGBSurface() and has those 
 * semantics.  You can also pass SDL_RLEACCEL in the flags parameter and
 * SDL will try to RLE accelerate colorkey and alpha blits in the resulting
 * surface.
 *
 * This function is used internally by SDL_DisplayFormat().
 */
extern DECLSPEC SDL_Surface * SDLCALL SDL_ConvertSurface
			(SDL_Surface *src, SDL_PixelFormat *fmt, Uint32 flags);

extern DECLSPEC SDL_Surface * SDLCALL SDL_DisplayFormat(SDL_Surface *surface);

Uint8 SDL_FindColor(SDL_Palette *pal, Uint8 r, Uint8 g, Uint8 b);
int SDL_SetAlpha (SDL_Surface *surface, Uint32 flag, Uint8 value);

#endif
