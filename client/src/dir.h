#ifndef DIR_H
#define DIR_H

#include <sys/stat.h>

static const char SYNC_DIR[] = "sync_dir_";

typedef struct stat stat_t;

void create_user_dir(char* username);
int check_dir_exists(char const* dir);

#endif