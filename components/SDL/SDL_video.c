#include "SDL_video.h"
#include "SDL.h"
//#include "SDL_blit.h"
#include "spi_lcd.h"

#define SPI_BUS TFT_VSPI_HOST

SDL_Surface* primary_surface;

int SDL_LockSurface(SDL_Surface *surface)
{
    return 0;
}

void SDL_UnlockSurface(SDL_Surface* surface)
{

}

void SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h)
{
    SDL_Flip(screen);
}

void SetPF(int depth, SDL_PixelFormat *pf)
{
	switch(depth)
	{
		case 32:
			pf->Rloss = 0; pf->Gloss = 0; pf->Bloss = 0; pf->Aloss = 0;
			pf->Rmask = 0xFF000000; 
			pf->Gmask = 0x00FF0000; 
			pf->Bmask = 0x0000FF00; 
			pf->Amask = 0x000000FF;
			pf->Rshift = 24; 
			pf->Gshift = 16; 
			pf->Bshift = 8; 
			pf->Ashift = 0;	
			break;
		case 16:
			pf->Rloss = 8-(depth/3);
			pf->Gloss = 8-(depth/3)-(depth%3);
			pf->Bloss = 8-(depth/3);
			pf->Rshift = ((depth/3)+(depth%3))+(depth/3);
			pf->Gshift = (depth/3);
			pf->Bshift = 0;		

			//pf->Rmask = 0x7C00;
			//pf->Gmask = 0x03E0;
			//pf->Bmask = 0x001F;

			pf->Rmask = ((0xFF>>pf->Rloss)<<pf->Rshift);
			pf->Gmask = ((0xFF>>pf->Gloss)<<pf->Gshift);
			pf->Bmask = ((0xFF>>pf->Bloss)<<pf->Bshift);			
			pf->Amask = 0;
			break;
		case 8:
			pf->Rloss = 8;
			pf->Gloss = 8;
			pf->Bloss = 8;
			pf->Aloss = 8;
			pf->Rshift = 0;
			pf->Gshift = 0;
			pf->Bshift = 0;
			pf->Ashift = 0;
			pf->Rmask = 0;
			pf->Gmask = 0;
			pf->Bmask = 0;
			pf->Amask = 0;
			break;	
	}	
}

SDL_VideoInfo *SDL_GetVideoInfo(void)
{
	SDL_Color col[] = {{0, 0, 0, 0}};
    SDL_Palette pal =  {.ncolors=1, .colors=&col};
    SDL_VideoInfo *info = malloc(sizeof(SDL_VideoInfo));
    info->hw_available = 0;
    info->wm_available = 0;
    info->video_mem = 1024*2;
    info->blit_hw = 0;
    info->blit_hw_CC = 0;
    info->blit_hw_A = 0;
    info->blit_sw = 0;
    info->blit_sw_CC = 0;
    info->blit_sw_A = 0;
    info->vfmt = NULL;
    info->vfmt = malloc(sizeof(SDL_PixelFormat));
	SetPF(lcd_bpp, info->vfmt);
	info->vfmt->palette = lcd_bpp == 8 ? &pal : NULL;
    info->vfmt->BitsPerPixel = lcd_bpp;
    info->vfmt->BytesPerPixel = lcd_bpp/8;

    info->vfmt->colorkey = 0;
    info->vfmt->alpha = 0;

    return info;
}

char *SDL_VideoDriverName(char *namebuf, int maxlen)
{
    return "Gadget Workbench - Awesome SPI TFT Driver";
}


SDL_Rect **SDL_ListModes(SDL_PixelFormat *format, Uint32 flags)
{
    SDL_Rect mode[1] = {{0,0,320,200}};
    return &mode;
}

void SDL_WM_SetCaption(const char *title, const char *icon)
{
    printf("Title: %s\n", title);
}

char *SDL_GetKeyName(SDLKey key)
{
    return (char *)"";
}

Uint32 SDL_WasInit(Uint32 flags)
{
    //return (tft == NULL);
	return 0;
}

int SDL_InitSubSystem(Uint32 flags)
{
    if(flags == SDL_INIT_VIDEO)
    {
    	spi_lcd_init();
        //SDL_CreateRGBSurface(0, 320, 200, 32, 0xFF000000,0xFF0000,0xFF00,0xFF);
    }
    return 0; // 0 = OK, -1 = Error
}


SDL_Surface *SDL_CreateRGBSurface(Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
	Check("Create Surface");
printf("Surface Width: %d Height: %d Depth: %d\n", width, height, depth);
    SDL_Surface *surface = (SDL_Surface *)calloc(1, sizeof(SDL_Surface));
    SDL_Rect rect = { .x=0, .y=0, .w=width, .h=height};
    SDL_Color *col = (SDL_Color *)malloc(256 * sizeof(SDL_Color));
    SDL_Palette pal =  {.ncolors=255, .colors=col};
    SDL_PixelFormat* pf = (SDL_PixelFormat*)calloc(1, sizeof(SDL_PixelFormat));
	pf->palette = depth == 8 ? &pal : NULL;
	pf->BitsPerPixel = depth;//8;
	pf->BytesPerPixel = depth/8;
	
	SetPF(depth, pf);

	pf->colorkey = 0;
	pf->alpha = 0;

    surface->flags = flags;
    surface->format = pf;
    surface->w = width;
    surface->h = height;
    surface->pitch = width*(depth/8);
    surface->clip_rect = rect;
    surface->refcount = 1;
    surface->pixels = heap_caps_malloc(width*height*(depth/8)/*1*/, MALLOC_CAP_SPIRAM);
    surface->map = malloc(sizeof(SDL_BlitMap));
    surface->map->sw_blit = SDL_SoftBlit;
    surface->map->sw_data = malloc(sizeof(pub_swaccel));
    surface->map->sw_data->blit = SDL_BlitCopy;


    memset(surface->pixels,0,(width*height/sizeof(surface->pixels)));
	//if(primary_surface == NULL)
	//	primary_surface = surface;
    return surface;
}

/*
 * Create an RGB surface from an existing memory buffer
 */
SDL_Surface * SDL_CreateRGBSurfaceFrom (void *pixels,
			int width, int height, int depth, int pitch,
			Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
	SDL_Surface *surface;

	surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 0, 0, depth,
	                               Rmask, Gmask, Bmask, Amask);
	if ( surface != NULL ) {
		surface->flags |= SDL_PREALLOC;
		surface->pixels = pixels;
		surface->w = width;
		surface->h = height;
		surface->pitch = pitch;
		//SDL_SetClipRect(surface, NULL);
	}
	return(surface);
}

int SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color)
{
    if(dst == NULL )//|| dst->sprite == NULL)
    {
		printf("SDL_FillRect with NULL surface\n");
        // Draw directly on screen
    	//if(dstrect == NULL)
    		//TFT_fillWindow(TFT_BLACK);
    	//else
    		//TFT_fillRect(dstrect->x, dstrect->y, dstrect->w, dstrect->h, TFT_BLACK);
    } else {
    	if(dstrect != NULL)
    	{
			if(lcd_bpp == 32)
			{
				uint32_t *pixels = dst->pixels;
				for(int y = dstrect->y; y < dstrect->y + dstrect->h; y++)
					for(int x = dstrect->x; x < dstrect->x + dstrect->w; x++)
						pixels[x + y * dst->w ] = color;
			} else
			{
				uint8_t *pixels = dst->pixels;
				for(int y = dstrect->y; y < dstrect->y + dstrect->h; y++)
					for(int x = dstrect->x; x < dstrect->x + dstrect->w; x++)
						pixels[x + y * dst->w ] = (uint8_t)color;
			}
			
		} else {
			printf("SDL_FillRect with NULL rect\n");
    		memset(dst->pixels, (unsigned char)color, dst->pitch*dst->h);
    	}
    }
    return 0;
}

SDL_Surface *SDL_GetVideoSurface(void)
{
    if(primary_surface == NULL)
    {	
		int width = 320;
		int height = 240;
		int depth = lcd_bpp;
		
		SDL_Surface *surface = (SDL_Surface *)calloc(1, sizeof(SDL_Surface));
		SDL_Rect rect = { .x=0, .y=0, .w=width, .h=height};
		SDL_Color col[] = {{0, 0, 0, 0}};
		SDL_Palette pal =  {.ncolors=1, .colors=&col};
		SDL_PixelFormat* pf = (SDL_PixelFormat*)calloc(1, sizeof(SDL_PixelFormat));
		pf->palette = depth == 8 ? &pal : NULL;
		pf->BitsPerPixel = depth;
		pf->BytesPerPixel = depth/8;
		SetPF(depth, pf);
		pf->colorkey = 0;
		pf->alpha = 0;

		surface->flags = 0;//flags;
		surface->format = pf;
		surface->w = width;
		surface->h = height;
		surface->pitch = width*(depth/8);
		surface->clip_rect = rect;
		surface->refcount = 1;
		surface->pixels = currFbPtr;
		//heap_caps_malloc(width*height*(depth/8)/*1*/, MALLOC_CAP_SPIRAM);
		surface->map = malloc(sizeof(SDL_BlitMap));
		surface->map->sw_blit = SDL_SoftBlit;
		surface->map->sw_data = malloc(sizeof(pub_swaccel));
		surface->map->sw_data->blit = SDL_BlitCopy;
		primary_surface = surface;	
	}
    return primary_surface;
}

Uint32 SDL_MapRGB(const SDL_PixelFormat * const format,
 const Uint8 r, const Uint8 g, const Uint8 b)
{
	if ( format->palette == NULL ) {
		return (r >> format->Rloss) << format->Rshift
		       | (g >> format->Gloss) << format->Gshift
		       | (b >> format->Bloss) << format->Bshift
		       | format->Amask;
	} else {
		return SDL_FindColor(format->palette, r, g, b);
	}/*
    if(fmt->BitsPerPixel == 16)
    {
        uint16_t bb = (b >> 3) & 0x1f;
        uint16_t gg = ((g >> 2) & 0x3f) << 5;
        uint16_t rr = ((r >> 3) & 0x1f) << 11;
        return (Uint32) (rr | gg | bb);        
    }
	if(fmt->BitsPerPixel == 32)
    {
        return (Uint32) (r<<24 | g<<16 | b<<8);        
    }
    return (Uint32)0;*/
}

int SDL_SetColors(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors)
{
	for(int i = firstcolor; i < firstcolor+ncolors; i++)
	{
		int v=((colors[i].r>>3)<<11)+((colors[i].g>>2)<<5)+(colors[i].b>>3);
		lcdpal[i]=(v>>8)+(v<<8);
	}
	return 1;
}

SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags)
{
	return SDL_GetVideoSurface();
}

void SDL_FreeSurface(SDL_Surface *surface)
{
    free(surface->pixels);
    free(surface->format);
    surface->refcount = 0;
}

void SDL_QuitSubSystem(Uint32 flags)
{

}

int SDL_Flip(SDL_Surface *screen)
{
	spi_lcd_send_boarder(screen->pixels, 20);
	//spi_lcd_send(screen->pixels);
	return 0;
}

int SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags)
{
	if(bpp == 8 || bpp == 16 || bpp == 32)
		return 1;
	return 0;
}

SemaphoreHandle_t display_mutex = NULL;

void SDL_LockDisplay()
{
    if (display_mutex == NULL)
    {
        printf("Creating display mutex.\n");
        display_mutex = xSemaphoreCreateMutex();
        if (!display_mutex) 
            abort();
        //xSemaphoreGive(display_mutex);
    }
/*
	while(1)
	{
		if (xSemaphoreTake(display_mutex, 10000 / portTICK_RATE_MS) == pdTRUE)
			break;
		printf("Timeout waiting for display lock - trying again.\n");
		vTaskDelay( 200 );	
		taskYIELD(); 
	}
*/
    if (!xSemaphoreTake(display_mutex, 60000 / portTICK_RATE_MS))
    {
        printf("Timeout waiting for display lock.\n");
        abort();
    }
    //printf("Lock \n");   
}

void SDL_UnlockDisplay()
{
    if (!display_mutex) 
        abort();
    if (!xSemaphoreGive(display_mutex))
        abort();

    //printf("Unlock\n");
    //taskYIELD();
}

/*
 * Set the physical and/or logical colormap of a surface:
 * Only the screen has a physical colormap. It determines what is actually
 * sent to the display.
 * The logical colormap is used to map blits to/from the surface.
 * 'which' is one or both of SDL_LOGPAL, SDL_PHYSPAL
 *
 * Return nonzero if all colours were set as requested, or 0 otherwise.
 */
int SDL_SetPalette(SDL_Surface *screen, int which,
		   SDL_Color *colors, int firstcolor, int ncolors)
{
    return SDL_SetColors(screen, colors, firstcolor, ncolors);
}

/*
int SDL_UpperBlit (SDL_Surface *src, SDL_Rect *srcrect,
		   SDL_Surface *dst, SDL_Rect *dstrect)
{

    return 0;
}
*/

/* 
 * Set up a blit between two surfaces -- split into three parts:
 * The upper part, SDL_UpperBlit(), performs clipping and rectangle 
 * verification.  The lower part is a pointer to a low level
 * accelerated blitting function.
 *
 * These parts are separated out and each used internally by this 
 * library in the optimimum places.  They are exported so that if
 * you know exactly what you are doing, you can optimize your code
 * by calling the one(s) you need.
 */
int SDL_LowerBlit (SDL_Surface *src, SDL_Rect *srcrect,
				SDL_Surface *dst, SDL_Rect *dstrect)
{
	SDL_blit do_blit;
	SDL_Rect hw_srcrect;
	SDL_Rect hw_dstrect;

	/* Check to make sure the blit mapping is valid */
	if ( (src->map->dst != dst) ||
             (src->map->dst->format_version != src->map->format_version) ) {
		//if ( SDL_MapSurface(src, dst) < 0 ) {
		//	return(-1);
		//}
	}

	/* Figure out which blitter to use */
    /*
	if ( (src->flags & SDL_HWACCEL) == SDL_HWACCEL ) {
		if ( src == SDL_VideoSurface ) {
			hw_srcrect = *srcrect;
			hw_srcrect.x += current_video->offset_x;
			hw_srcrect.y += current_video->offset_y;
			srcrect = &hw_srcrect;
		}
		if ( dst == SDL_VideoSurface ) {
			hw_dstrect = *dstrect;
			hw_dstrect.x += current_video->offset_x;
			hw_dstrect.y += current_video->offset_y;
			dstrect = &hw_dstrect;
		}
		do_blit = src->map->hw_blit;
	} else {
    */    
		do_blit = src->map->sw_blit;
	//}
	return(do_blit(src, srcrect, dst, dstrect));
}


int SDL_UpperBlit (SDL_Surface *src, SDL_Rect *srcrect,
		   SDL_Surface *dst, SDL_Rect *dstrect)
{
        SDL_Rect fulldst;
	int srcx, srcy, w, h;

	/* Make sure the surfaces aren't locked */
	if ( ! src || ! dst ) {
		SDL_SetError("SDL_UpperBlit: passed a NULL surface");
		return(-1);
	}
	if ( src->locked || dst->locked ) {
		SDL_SetError("Surfaces must not be locked during blit");
		return(-1);
	}

	/* If the destination rectangle is NULL, use the entire dest surface */
	if ( dstrect == NULL ) {
	        fulldst.x = fulldst.y = 0;
		dstrect = &fulldst;
	}

	/* clip the source rectangle to the source surface */
	if(srcrect) {
	        int maxw, maxh;
	
		srcx = srcrect->x;
		w = srcrect->w;
		if(srcx < 0) {
		        w += srcx;
			dstrect->x -= srcx;
			srcx = 0;
		}
		maxw = src->w - srcx;
		if(maxw < w)
			w = maxw;

		srcy = srcrect->y;
		h = srcrect->h;
		if(srcy < 0) {
		        h += srcy;
			dstrect->y -= srcy;
			srcy = 0;
		}
		maxh = src->h - srcy;
		if(maxh < h)
			h = maxh;
	    
	} else {
	        srcx = srcy = 0;
		w = src->w;
		h = src->h;
	}

	/* clip the destination rectangle against the clip rectangle */
	{
	        SDL_Rect *clip = &dst->clip_rect;
		int dx, dy;

		dx = clip->x - dstrect->x;
		if(dx > 0) {
			w -= dx;
			dstrect->x += dx;
			srcx += dx;
		}
		dx = dstrect->x + w - clip->x - clip->w;
		if(dx > 0)
			w -= dx;

		dy = clip->y - dstrect->y;
		if(dy > 0) {
			h -= dy;
			dstrect->y += dy;
			srcy += dy;
		}
		dy = dstrect->y + h - clip->y - clip->h;
		if(dy > 0)
			h -= dy;
	}

	if(w > 0 && h > 0) {
	        SDL_Rect sr;
	        sr.x = srcx;
		sr.y = srcy;
		sr.w = dstrect->w = w;
		sr.h = dstrect->h = h;
		return SDL_LowerBlit(src, &sr, dst, dstrect);
	}
	dstrect->w = dstrect->h = 0;
	return 0;
}

void SDL_GetRGBA(Uint32 pixel, const SDL_PixelFormat * const fmt,
		 Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a)
{
	if ( fmt->palette == NULL ) {
	        /*
		 * This makes sure that the result is mapped to the
		 * interval [0..255], and the maximum value for each
		 * component is 255. This is important to make sure
		 * that white is indeed reported as (255, 255, 255),
		 * and that opaque alpha is 255.
		 * This only works for RGB bit fields at least 4 bit
		 * wide, which is almost always the case.
		 */
	        unsigned v;
		v = (pixel & fmt->Rmask) >> fmt->Rshift;
		*r = (v << fmt->Rloss) + (v >> (8 - (fmt->Rloss << 1)));
		v = (pixel & fmt->Gmask) >> fmt->Gshift;
		*g = (v << fmt->Gloss) + (v >> (8 - (fmt->Gloss << 1)));
		v = (pixel & fmt->Bmask) >> fmt->Bshift;
		*b = (v << fmt->Bloss) + (v >> (8 - (fmt->Bloss << 1)));
		if(fmt->Amask) {
		        v = (pixel & fmt->Amask) >> fmt->Ashift;
			*a = (v << fmt->Aloss) + (v >> (8 - (fmt->Aloss << 1)));
		} else {
		        *a = SDL_ALPHA_OPAQUE;
                }
	} else {
		*r = fmt->palette->colors[pixel].r;
		*g = fmt->palette->colors[pixel].g;
		*b = fmt->palette->colors[pixel].b;
		*a = SDL_ALPHA_OPAQUE;
	}
}

void SDL_GetRGB(Uint32 pixel, const SDL_PixelFormat * const fmt,
                Uint8 *r,Uint8 *g,Uint8 *b)
{
	if ( fmt->palette == NULL ) {
	        /* the note for SDL_GetRGBA above applies here too */
	        unsigned v;
		v = (pixel & fmt->Rmask) >> fmt->Rshift;
		*r = (v << fmt->Rloss) + (v >> (8 - (fmt->Rloss << 1)));
		v = (pixel & fmt->Gmask) >> fmt->Gshift;
		*g = (v << fmt->Gloss) + (v >> (8 - (fmt->Gloss << 1)));
		v = (pixel & fmt->Bmask) >> fmt->Bshift;
		*b = (v << fmt->Bloss) + (v >> (8 - (fmt->Bloss << 1)));
	} else {
		*r = fmt->palette->colors[pixel].r;
		*g = fmt->palette->colors[pixel].g;
		*b = fmt->palette->colors[pixel].b;
	}
}

/* Find the pixel value corresponding to an RGBA quadruple */
Uint32 SDL_MapRGBA
(const SDL_PixelFormat * const format,
 const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a)
{
	if ( format->palette == NULL ) {
	        return (r >> format->Rloss) << format->Rshift
		    | (g >> format->Gloss) << format->Gshift
		    | (b >> format->Bloss) << format->Bshift
		    | ((a >> format->Aloss) << format->Ashift & format->Amask);
	} else {
		return SDL_FindColor(format->palette, r, g, b);
	}
}

/*
 * Set the color key in a blittable surface
 */
int SDL_SetColorKey (SDL_Surface *surface, Uint32 flag, Uint32 key)
{
	/* Sanity check the flag as it gets passed in */
	if ( flag & SDL_SRCCOLORKEY ) {
		//if ( flag & (SDL_RLEACCEL|SDL_RLEACCELOK) ) {
		//	flag = (SDL_SRCCOLORKEY | SDL_RLEACCELOK);
		//} else {
			flag = SDL_SRCCOLORKEY;
		//}
	} else {
		flag = 0;
	}

	/* Optimize away operations that don't change anything */
	if ( (flag == (surface->flags & (SDL_SRCCOLORKEY|SDL_RLEACCELOK))) &&
	     (key == surface->format->colorkey) ) {
		return(0);
	}

	/* UnRLE surfaces before we change the colorkey */
	//if ( surface->flags & SDL_RLEACCEL ) {
	//        SDL_UnRLESurface(surface, 1);
	//}

	if ( flag ) {
		//SDL_VideoDevice *video = current_video;
		//SDL_VideoDevice *this  = current_video;


		surface->flags |= SDL_SRCCOLORKEY;
		surface->format->colorkey = key;
		/*
		if ( (surface->flags & SDL_HWACCEL) == SDL_HWACCEL ) {
			if ( (video->SetHWColorKey == NULL) ||
			     (video->SetHWColorKey(this, surface, key) < 0) ) {
				surface->flags &= ~SDL_HWACCEL;
			}
		}
		if ( flag & SDL_RLEACCELOK ) {
			surface->flags |= SDL_RLEACCELOK;
		} else {
		*/	
			surface->flags &= ~SDL_RLEACCELOK;
		//}
	} else {
		surface->flags &= ~(SDL_SRCCOLORKEY|SDL_RLEACCELOK);
		surface->format->colorkey = 0;
	}
	SDL_InvalidateMap(surface->map);
	return(0);
}

/* 
 * Convert a surface into the specified pixel format.
 */
SDL_Surface * SDL_ConvertSurface (SDL_Surface *surface,
					SDL_PixelFormat *format, Uint32 flags)
{
	return surface;
}

/*
 * Match an RGB value to a particular palette index
 */
Uint8 SDL_FindColor(SDL_Palette *pal, Uint8 r, Uint8 g, Uint8 b)
{
	if(pal == NULL)
		return 0;
	/* Do colorspace distance matching */
	unsigned int smallest;
	unsigned int distance;
	int rd, gd, bd;
	int i;
	Uint8 pixel=0;
		
	smallest = ~0;
	for ( i=0; i<pal->ncolors; ++i ) {
		rd = pal->colors[i].r - r;
		gd = pal->colors[i].g - g;
		bd = pal->colors[i].b - b;
		distance = (rd*rd)+(gd*gd)+(bd*bd);
		if ( distance < smallest ) {
			pixel = i;
			if ( distance == 0 ) { /* Perfect match! */
				break;
			}
			smallest = distance;
		}
	}
	return(pixel);
}

/* This function sets the alpha channel of a surface */
int SDL_SetAlpha (SDL_Surface *surface, Uint32 flag, Uint8 value)
{
	Uint32 oldflags = surface->flags;
	Uint32 oldalpha = surface->format->alpha;

	/* Sanity check the flag as it gets passed in */
	if ( flag & SDL_SRCALPHA ) {
		//if ( flag & (SDL_RLEACCEL|SDL_RLEACCELOK) ) {
		//	flag = (SDL_SRCALPHA | SDL_RLEACCELOK);
		//} else {
			flag = SDL_SRCALPHA;
		//}
	} else {
		flag = 0;
	}

	/* Optimize away operations that don't change anything */
	if ( (flag == (surface->flags & (SDL_SRCALPHA|SDL_RLEACCELOK))) &&
	     (!flag || value == oldalpha) ) {
		return(0);
	}

	//if(!(flag & SDL_RLEACCELOK) && (surface->flags & SDL_RLEACCEL))
	//	SDL_UnRLESurface(surface, 1);

	if ( flag ) {
		//SDL_VideoDevice *video = current_video;
		//SDL_VideoDevice *this  = current_video;

		surface->flags |= SDL_SRCALPHA;
		surface->format->alpha = value;
	//	if ( (surface->flags & SDL_HWACCEL) == SDL_HWACCEL ) {
	//		if ( (video->SetHWAlpha == NULL) ||
	//		     (video->SetHWAlpha(this, surface, value) < 0) ) {
	//			surface->flags &= ~SDL_HWACCEL;
	//		}
	//	}
	//	if ( flag & SDL_RLEACCELOK ) {
	//	        surface->flags |= SDL_RLEACCELOK;
	//	} else {
		        surface->flags &= ~SDL_RLEACCELOK;
	//	}
	} else {
		surface->flags &= ~SDL_SRCALPHA;
		surface->format->alpha = SDL_ALPHA_OPAQUE;
	}
	/*
	 * The representation for software surfaces is independent of
	 * per-surface alpha, so no need to invalidate the blit mapping
	 * if just the alpha value was changed. (If either is 255, we still
	 * need to invalidate.)
	 */
	if((surface->flags & SDL_HWACCEL) == SDL_HWACCEL
	   || oldflags != surface->flags
	   || (((oldalpha + 1) ^ (value + 1)) & 0x100))
		SDL_InvalidateMap(surface->map);
	return(0);
}

/* 
 * Convert a surface into the video pixel format.
 */
SDL_Surface * SDL_DisplayFormat (SDL_Surface *surface)
{

	return(surface);
}
