/* LZ.C
  This file contains the functions which decompress the graphics
  data from Keen 1.
*/
#include "keen.h"
#include "lz.fdh"

#define LZ_STARTBITS        9
#define LZ_MAXBITS          12
#define LZ_ERRORCODE        256
#define LZ_EOFCODE          257
#define LZ_DICTSTARTCODE    258

#define LZ_MAXDICTSIZE      ((1<<LZ_MAXBITS)+1)
#define LZ_MAXSTRINGSIZE    72

unsigned char *lz_outbuffer;

typedef struct stLZDictionaryEntry
{
  int stringlen;
  unsigned char string[LZ_MAXSTRINGSIZE];
} stLZDictionaryEntry;

stLZDictionaryEntry *lzdict[LZ_MAXDICTSIZE];

// reads a word of length numbits from file lzfile.
unsigned int lz_readbits(FILE *lzfile, unsigned char numbits, unsigned char reset)
{
static int mask, byte;
unsigned char bitsread;
unsigned int dat;

	if (reset)
	{
		mask = 0;
		byte = 0;
		return 0;
	}

	bitsread = 0;
	dat = 0;
	do
	{        
        if (!mask)
        {
           byte = __fgetc(lzfile);
           mask = 0x80;
        }
		
        if (byte & mask)
        {
          dat |= 1 << ((numbits - bitsread) - 1);
        }

        mask >>= 1;
        bitsread++;
     } while(bitsread<numbits);

     return dat;
}

// writes dictionary entry 'entry' to the output buffer
void lz_outputdict(int entry)
{
int i;

  for(i=0;i<lzdict[entry]->stringlen;i++)
  {
    *lz_outbuffer = lzdict[entry]->string[i];
    lz_outbuffer++;
  }
}

// decompresses LZ data from open file lzfile into buffer outbuffer
// returns nonzero if an error occurs
char lz_decompress(FILE *lzfile, unsigned char *outbuffer)
{
int i;
int numbits;
unsigned int dictindex, maxdictindex;
unsigned int lzcode,lzcode_save,lastcode;
char addtodict;

	/* allocate memory for the LZ dictionary */
	for(i=0;i<LZ_MAXDICTSIZE;i++)
	{
		lzdict[i] = malloc(sizeof(stLZDictionaryEntry));
		if (!lzdict[i])
		{
			lprintf("lz_decompress(): unable to allocate memory for dictionary!\n");
			//for(i--;i>=0;i--) free(lzdict[i]);
			
			return 1;
		}
	}

	/* initilize the dictionary */

	// entries 0-255 start with a single character corresponding
	// to their entry number
	for(i=0;i<256;i++)
	{
		lzdict[i]->stringlen = 1;
		lzdict[i]->string[0] = i;
	}
	// 256+ start undefined
	for(i=256;i<LZ_MAXDICTSIZE;i++)
	{
		lzdict[i]->stringlen = 0;
	}

	// reset readbits
	lz_readbits(NULL, 0, 1);

	// set starting # of bits-per-code
	numbits = LZ_STARTBITS;
	maxdictindex = (1 << numbits) - 1;

	// point the global pointer to the buffer we were passed
	lz_outbuffer = outbuffer;

	// setup where to start adding strings to the dictionary
	dictindex = LZ_DICTSTARTCODE;
	addtodict = 1;                    // enable adding to dictionary

	// read first code
	lastcode = lz_readbits(lzfile, numbits, 0);
	lz_outputdict(lastcode);
	do
	{
		// read the next code from the compressed data stream
		lzcode = lz_readbits(lzfile, numbits, 0);
		lzcode_save = lzcode;
		
		if (lzcode==LZ_ERRORCODE || lzcode==LZ_EOFCODE)
		{
			break;
		}
		
		// if the code is present in the dictionary,
		// lookup and write the string for that code, then add the
		// last string + the first char of the just-looked-up string
		// to the dictionary at dictindex
		
		// if not in dict, add the last string + the first char of the
		// last string to the dictionary at dictindex (which will be equal
		// to lzcode), then lookup and write string lzcode.
		
		if (lzdict[lzcode]->stringlen==0)
		{  // code is not present in dictionary             
			lzcode = lastcode;
		}
		
		if (addtodict)     // room to add more entries to the dictionary?
		{
			// copies string lastcode to string dictindex, then
			// concatenates the first character of string lzcode.
			for(i=0;i<lzdict[lastcode]->stringlen;i++)
			{
				lzdict[dictindex]->string[i] = lzdict[lastcode]->string[i];
			}
			lzdict[dictindex]->string[i] = lzdict[lzcode]->string[0];
			lzdict[dictindex]->stringlen = (lzdict[lastcode]->stringlen + 1);
			
			// ensure we haven't overflowed the buffer
			if (lzdict[dictindex]->stringlen >= (LZ_MAXSTRINGSIZE-1))
			{
				lprintf("lz_decompress(): lzdict[%d]->stringlen is too long...max length is %d\n", dictindex, LZ_MAXSTRINGSIZE);
				return 1;
			}
			
			if (++dictindex >= maxdictindex)
			{ // no more entries can be specified with current code bit-width
				if (numbits < LZ_MAXBITS)
				{  // increase width of codes
					numbits++;
					maxdictindex = (1 << numbits) - 1;
				}
				else
				{
					// reached maximum bit width, can't increase.
					// use the final entry (4095) before we shut off
					// adding items to the dictionary.
					if (dictindex>=(LZ_MAXDICTSIZE-1)) addtodict = 0;
				}
			}
		}
		
		// write the string associated with the original code read.
		// if the code wasn't present, it now should have been added.
		lz_outputdict(lzcode_save);
		
		lastcode = lzcode_save;
	} while(1);

	/* free the memory used by the LZ dictionary */
	for(i=0;i<LZ_MAXDICTSIZE;i++)
	{
		free(lzdict[i]);
	}

	return 0;
}

