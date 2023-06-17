#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

static void	err_doit(int, const char*, va_list);

// Nonfatal error related to system call
// Print message and return
void err_ret(const char* fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
}

// Fatal error related to system call
// Print message and terminate
void err_sys(const char* fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	exit(1);
}

// Fatal error related to system call
// Print message, dump core, and terminate
void err_dump(const char* fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	abort();		// Dump core and terminate
	exit(1);		// Shouldn't get here
}

// Nonfatal error unrelated to system call
// Print message and return
void err_msg(const char* fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
}

// Fatal error unrelated to system call
// Print message and terminate
void err_quit(const char* fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
	exit(1);
}

// Print message and return to caller
// Caller specifies "errnoflag"
static void err_doit(int errnoflag, const char* fmt, va_list ap)
{
	char buf[MAXLINE + 1];

	vsnprintf(buf, MAXLINE, fmt, ap);
	int n = strlen(buf);
	int errno_save = errno;

	if (errnoflag)
		snprintf(buf + n, MAXLINE - n, ": %s", strerror(errno_save));

	strcat(buf, "\n");
	fflush(stdout);		 
	fputs(buf, stderr);
	fflush(stderr);
}