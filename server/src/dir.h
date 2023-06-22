#ifndef USER_H
#define USER_H

#include <sys/stat.h>

typedef struct stat stat_t;

typedef struct user
{
  char *username;
  char *dir;
} user_t;

user_t createUserDir(const char *);

bool checkDirExists(const char *);
// stat_t getDir(const char*);

#endif