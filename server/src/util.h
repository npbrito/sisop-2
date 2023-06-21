#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

ssize_t Readn(int fd, void* ptr, size_t nbytes);
uint16_t str_to_port(const char* str);

#endif