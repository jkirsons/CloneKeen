
typedef struct png_color_struct
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} png_color;

/*
 * This file is part of the Scale2x project.
 *
 * Copyright (C) 2003 Andrea Mazzoleni
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details. 
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * This is the source of a simple command line tool which uses the fast
 * implementation of the Scale effects.
 *
 * You can find an high level description of the effects at :
 *
 * http://scale2x.sourceforge.net/
 */

#include "scalebit.h"
//#include "file.h"
#include "portable.h"

#include <zlib.h>

#include <stdlib.h>
#include <stdio.h>

int file_process(const char* file0, const char* file1, int opt_scale_x, int opt_scale_y, int opt_crc)
{
	unsigned pixel = 0;
	unsigned width = 0;
	unsigned height = 0;
	unsigned char* src_ptr;
	unsigned src_slice;
	unsigned char* dst_ptr;
	unsigned dst_slice;
	int type;
	int channel;
	png_color* palette;
	unsigned palette_size;
/*
	if (file_read(file0, &src_ptr, &src_slice, &pixel, &width, &height, &type, &channel, &palette, &palette_size, 1) != 0) {
		goto err;
	}
*/
	if (scale_precondition(opt_scale_x * 100 + opt_scale_y, pixel, width, height) != 0) {
		fprintf(stderr, "Error in the size of the source bitmap. Generally this happen\n");
		fprintf(stderr, "when the bitmap is too small or when the width is not an exact\n");
		fprintf(stderr, "multiplier of 8 bytes.\n");
		goto err_src;
	}

	dst_slice = width * pixel * opt_scale_x;
	dst_ptr = malloc(dst_slice * height * opt_scale_y);
	if (!dst_ptr) {
		fprintf(stderr, "Low memory.\n");
		goto err_src;
	}

	scale(opt_scale_x * 100 + opt_scale_y, dst_ptr, dst_slice, src_ptr, src_slice, pixel, width, height);
/*
	if (file_write(file1, dst_ptr, dst_slice, pixel, width * opt_scale_x, height * opt_scale_y, type, channel, palette, palette_size) != 0) {
		goto err_dst;
	}
*/
	if (opt_crc) {
		unsigned crc = crc32(0, dst_ptr, dst_slice * height * opt_scale_y);
		printf("%08x\n", crc);
	}

	free(dst_ptr);
	free(src_ptr);
	free(palette);

	return 0;

err_dst:
	free(dst_ptr);
err_src:
	free(src_ptr);
	free(palette);
err:
	return -1;
}

void version(void) {
//	printf(PACKAGE " v" VERSION " by Andrea Mazzoleni\n");
}

void usage(void) {
	version();
	printf("Fast implementation of the Scale2/3/4x effects\n");
#if defined(__GNUC__) && defined(__i386__)
	printf("(using Pentium MMX optimization)\n");
#endif
	printf("\nSyntax: scalex [-k N] FROM.png TO.png\n");
	printf("\nOptions:\n");
	printf("\t-k N\tSelect the scale factor. 2, 2x3, 2x4, 3 or 4. (default 2).\n");
	printf("\nMore info at http://scale2x.sourceforge.net/\n");
	exit(EXIT_FAILURE);
}

#ifdef HAVE_GETOPT_LONG
struct option long_options[] = {
	{"scale", 1, 0, 'k'},
	{"crc", 0, 0, 'c'},
	{"help", 0, 0, 'h'},
	{"version", 0, 0, 'v'},
	{0, 0, 0, 0}
};
#endif

#define OPTIONS "k:chv"
/*
int main(int argc, char* argv[]) {
	int opt_scale_x = 2;
	int opt_scale_y = 2;
	int opt_crc = 0;
	int c;

	opterr = 0;

	while ((c =
#ifdef HAVE_GETOPT_LONG
		getopt_long(argc, argv, OPTIONS, long_options, 0))
#else
		getopt(argc, argv, OPTIONS))
#endif
	!= EOF) {
		switch (c) {
			case 'h' :
				usage();
				exit(EXIT_SUCCESS);
			case 'v' :
				version();
				exit(EXIT_SUCCESS);
			case 'k' :
				if (strcmp(optarg, "2") == 0) {
					opt_scale_x = 2;
					opt_scale_y = 2;
				} else if (strcmp(optarg, "3") == 0) {
					opt_scale_x = 3;
					opt_scale_y = 3;
				} else if (strcmp(optarg, "4") == 0) {
					opt_scale_x = 4;
					opt_scale_y = 4;
				} else {
					if (sscanf(optarg, "%dx%d", &opt_scale_x, &opt_scale_y) != 2
						|| opt_scale_x < 1
						|| opt_scale_y < 1
					) {
						printf("Invalid -k option. Valid values are 2, 2x3, 2x4, 3 and 4.\n");
						exit(EXIT_FAILURE);
					}
				}
				break;
			case 'c' :
				opt_crc = 1;
				break;
			default:
				printf("Unknown option `%c'.\n", (char)optopt);
				exit(EXIT_FAILURE);
		} 
	}

	if (optind + 2 != argc) {
		usage();
		exit(EXIT_FAILURE);
	}

	if (file_process(argv[optind], argv[optind+1], opt_scale_x, opt_scale_y, opt_crc) != 0) {
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
*/
