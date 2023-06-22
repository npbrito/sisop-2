#ifndef DIR_H
#define DIR_H

#include <sys/stat.h>
#include "user.h"

#define SYNC_DIR "sync_dir/"

typedef struct stat stat_t;

void createUserDir(char *);

bool checkDirExists(char *);
// stat_t getDir(char*);

#endif