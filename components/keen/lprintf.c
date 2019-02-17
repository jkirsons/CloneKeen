#include <stdio.h>
#include <stdarg.h>
#include "keen.h"
#include "lprintf.fdh"

#define CKLOGFILENAME            "ck.log"
char crash_reason[2048];

void lprintf(const char *str, ...)
{
FILE *log;
va_list ar;
char buf[2048];

	va_start(ar, str);
	vsprintf(buf, str, ar);
	va_end(ar);
	
	fputs(buf, stdout);
	fflush(stdout);
	
	log = __fopen(CKLOGFILENAME, "a+");
	if (log)
	{
		fputs(buf, log);
		__fclose(log);
	}
}

void crash(const char *str, ...)
{
va_list ar;
char buf[2048];

	va_start(ar, str);
	vsprintf(buf, str, ar);
	va_end(ar);
	
	lprintf("> Crash! Kaboom! ** %s\n", buf);
	strcpy(crash_reason, buf);
	crashflag = 1;
}

// initilize the console that lprintf outputs to.
char lprintf_init(void)
{
	remove(CKLOGFILENAME);
	return 0;
}

void lprintf_close(void)
{
}
