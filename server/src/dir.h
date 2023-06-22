#ifndef DIR_H
#define DIR_H

#include <sys/stat.h>
#include "user.h"

static const char SYNC_DIR[] = "sync_dir/";

typedef struct stat stat_t;

void createUserDir(char *);

bool checkDirExists(const char *);
// stat_t getDir(char*);

#endif