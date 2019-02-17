/* SGRLE.C
  RLE compression and decompression functions for the SGRLE format...
  These are used for saved games (hence the name SG RLE). Could be used
  for other things too, I guess.

  After opening a file for decompression you must call sgrle_reset() before
  the first time you use sgrle_decompress().
*/
#include "keen.h"
#include "sgrle.fdh"


#define SGRLE_RLEMARKER         0xFE
#define SGRLE_MAXRUNLEN         0xFFF0
int sgrle_runlen;
unsigned char sgrle_runchar;

/* decompresses the next byte from file FP. */
/* used internally by sgrle_decompress(). */
unsigned char sgrle_get_next_byte(FILE *fp)
{
	// are we currently in a RLE run?
	if (sgrle_runlen)
	{
		// decrease length of RLE run and return the previously
		// read char for the run
		sgrle_runlen--;
		return sgrle_runchar;
	}
	else
	{	// not currently in a RLE run
		sgrle_runchar = __fgetc(fp);
		if (sgrle_runchar==SGRLE_RLEMARKER)
		{  // start of a RLE run
			sgrle_runlen = fgeti(fp);
			sgrle_runchar = __fgetc(fp);
			return sgrle_get_next_byte(fp);
		}
		else return sgrle_runchar;
	}
}

void sgrle_initdecompression(void)
{
	sgrle_runlen = 0;
}

/* decompresses nbytes bytes of SGRLE-compressed data from */
/* file pointer *fp to the memory area pointed to by *ptr. */
char sgrle_decompress(FILE *fp, unsigned char *ptr, unsigned long nbytes)
{
unsigned long i;
unsigned long bytes;

	bytes = fgetl(fp);
	if (bytes != nbytes)
	{
		crash("sgrle_decompress: bytes stored != bytes asked for ($%08x / $%08x)\n", bytes, nbytes);
		return 1;
	}

	sgrle_runlen = 0;
	
	for(i=0;i<nbytes;i++)
		ptr[i] = sgrle_get_next_byte(fp);
	return 0;
}

/* given a memory area *ptr of length nbytes, compresses the data */
/* using the SGRLE algorithm and saves it to file *fp */
void sgrle_compress(FILE *fp, unsigned char *ptr, unsigned long nbytes)
{
int byt;
unsigned long compress_index, run_ahead_index;
unsigned int runlength;
int readbyt;
unsigned int i;

	fputl(nbytes, fp);

	compress_index = 0;
	while(compress_index < nbytes)
	{
		// read a byte from the buffer
		readbyt = ptr[compress_index];

		/* is the next byte the same? if so find the length of the run */
		if ((compress_index+1 < nbytes) && ptr[compress_index+1]==readbyt)
		{
			/* find how long the run is (a run of byte readbyt) */
			run_ahead_index = (compress_index + 1);
			runlength = 1;
			do
			{
				byt = ptr[run_ahead_index];

				// the run is over when either the byte is different
				// or run_ahead_index is at the end of the buffer,
				// or runlength is approaching FFFF (max possible RLE run length)
				if (byt != readbyt || run_ahead_index >= nbytes || runlength >= SGRLE_MAXRUNLEN)
				{
					break;
				}
				run_ahead_index++;
				runlength++;
			} while(1);

			// it takes 4 bytes to code a RLE run, so if the run is less than
			// 4 bytes, it would actually be smaller if we didn't compress it
			if (runlength < 4 && readbyt != SGRLE_RLEMARKER)
			{
				// RLE run, but too small to bother with
				for(i=0;i<runlength;i++) __fputc(readbyt, fp);
			}
			else
			{
				// save a RLE run
				__fputc(SGRLE_RLEMARKER, fp);
				fputi(runlength, fp);
				__fputc(readbyt, fp);
			}
			// advance
			compress_index += runlength;
		}
		else
		{
			// next byte is different, this is not a run, it's just a single char
			if (readbyt != SGRLE_RLEMARKER)
			{
				__fputc(readbyt, fp);
			}
			else
			{
				// it's a single uncompressed byte which is equal to the RLE marker.
				// delimit it by placing it in a RLE run of length 1.
				__fputc(SGRLE_RLEMARKER, fp);
				fputi(1, fp);
				__fputc(readbyt, fp);
			}
			compress_index++;
		}
	}

}

/* resets the decompression engine. (must call this before the first
   time you use sgrle_decompress(), each time you open a new file) */
void sgrle_reset(void)
{
  sgrle_runlen = 0;
}
