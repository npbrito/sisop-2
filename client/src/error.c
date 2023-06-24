#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

static void	err_doit(int, char const*, va_list);

// Nonfatal error related to system call
// Print message and return
void err_ret(char const* fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
}

// Fatal error related to system call
// Print message and terminate
void err_sys(char const* fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

// Fatal error related to system call
// Print message, dump core, and terminate
void err_dump(char const* fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	abort();				// Dump core and terminate
	exit(EXIT_FAILURE);		// Shouldn't get here
}

// Nonfatal error unrelated to system call
// Print message and return
void err_msg(char const* fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
}

// Fatal error unrelated to system call
// Print message and terminate
void err_quit(char const* fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

// Print message and return to caller
// Caller specifies "errnoflag"
static void err_doit(int errnoflag, char const* fmt, va_list ap)
{
	char buf[MAXLINE+1];

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