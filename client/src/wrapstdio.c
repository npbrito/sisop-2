#include "error.h"
#include "wrapstdio.h"

ssize_t Getline(char** lineptr, size_t* n, FILE* stream)
{
	ssize_t c;

	if ( (c = getline(lineptr, n, stream)) == -1)
		err_sys("getline error");

	return c;
}