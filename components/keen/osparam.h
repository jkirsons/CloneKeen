
// determine if the operating system uses case-sensitive filenames
#ifdef TARGET_LNX
	#define CASE_SENSITIVE_FILESYSTEM
#endif

// Linux calls stricmp strcasecmp(), Windows calls strcasecmp stricmp().
#ifdef TARGET_LNX
	#define stricmp		strcasecmp
#endif

// when compiling with MSVC turns off some annoying warnings...
#ifdef MSVC
	#pragma warning(disable:4761)
	#pragma warning(disable:4018)
#endif
