
/*
	Platform specific code.
	See the "platform" directory for your specific implementation,
	or add your own here via a define which is only present on your
	target platform.
	
	This code is in C++ because some platforms (Haiku) require it.
*/


extern "C"
{
	int LoadDirectoryListing(char *path);
	const char *GetFileAtIndex(int fno);
	char GetIsDirectory(int fno);
	void FreeDirectoryListing(void);
	
	void platform_msgbox(const char *message);
	
	// these might be ACCESSED by the C++ code, they are not actually defined here.
	void lprintf(const char *fmt, ...);
	int KeenMain(int argc, char *argv[]);
};

/*
	select appropriate platform-specific module based on operating system
	slightly cheap hack
*/
#ifdef __HAIKU__
	#include "platform/haiku.cpp"
#elif defined(__unix__)
	#include "platform/linux.cpp"
#elif defined(WIN32)
	#include "platform/win32.cpp"
#else
	#include "platform/stub.cpp"
#endif
