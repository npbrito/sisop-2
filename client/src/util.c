#include <stdlib.h>
#include <errno.h>
#include "error.h"
#include "util.h"

uint16_t str_to_port(const char* str)
{
    char* end;
    errno = 0;
    long val = strtol(str, &end, 10);

    if (errno != 0 || val > UINT16_MAX || val < 0 || str == end || '\0' != *end)
        err_quit("invalid port number error");

    return (uint16_t) val;
}