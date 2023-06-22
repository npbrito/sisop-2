#include "error.h"
#include "wrapstdio.h"

char* Fgets(char* ptr, int n, FILE* stream)
{
	char *rptr;

	if ( (rptr = fgets(ptr, n, stream)) == NULL && ferror(stream))
		err_sys("fgets error");

	return rptr;
}

void Fputs(const char* ptr, FILE* stream)
{
	if (fputs(ptr, stream) == EOF)
		err_sys("fputs error");
}

ssize_t Getline(char** lineptr, size_t* n, FILE* stream)
{
	ssize_t c;

	if ( (c = getline(lineptr, n, stream)) == -1)
		err_sys("getline error");

	return c;
}