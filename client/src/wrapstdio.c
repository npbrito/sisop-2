#include "wrapstdio.h"

void Fputs(const char* ptr, FILE* stream)
{
	if (fputs(ptr, stream) == EOF)
		err_sys("fputs error");
}