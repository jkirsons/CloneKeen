
#define NUM_PHONY_FILES		6
static const char *phony_files[] =
{
	"PLATFORM/STUB.C",
	" ",
	"Platform-specific functions",
	"missing from this build!",
	" ",
	"See PLATFORM/STUB.C"
};

/*
void c------------------------------() {}
*/
/*
int main(int argc, char *argv[])
{
	return KeenMain(argc, argv);
}
*/
void platform_msgbox(const char *message)
{
	lprintf("%s\n", message);
}

/*
void c------------------------------() {}
*/

/* define some stub functions so it will at least
   compile on an unsupported platform */

// Load the contents of the given directory into memory,
// and return the number of files in the directory.
int LoadDirectoryListing(char *path)
{
	return NUM_PHONY_FILES;
}

// Return the name of the file at index "fno", from a list
// previously loaded via LoadDirectoryListing.
// If the index is out of range, return "<>".
const char *GetFileAtIndex(int fno)
{
	if (fno >= NUM_PHONY_FILES) return "<>";
	return phony_files[fno];
}

// Return nonzero if the file at index "fno" is a directory.
char GetIsDirectory(int fno)
{
	return 1;
}

// Cleanup/do any freeing required when we are done with a directory listing.
void FreeDirectoryListing(void) { }
	

