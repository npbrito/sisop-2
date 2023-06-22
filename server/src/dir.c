#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include "error.h"
#include "dir.h"

void createUserDir(char *username)
{
  char *dir = malloc(strlen(SYNC_DIR) + strlen(username) + 1);
  strcpy(dir, SYNC_DIR);
  strcat(dir, username);

  if (!checkDirExists(SYNC_DIR))
  {
    mkdir(SYNC_DIR, 0700);
  }

  int n = mkdir(dir, 0700);

  free(dir);

  if (n != 0)
  {
    err_sys("createUserDir error");
  }
}

bool checkDirExists(char *dir)
{
  struct stat st;
  if (stat(dir, &st) == 0)
  {
    if (S_ISDIR(st.st_mode))
    {
      return true;
    }
  }
  return false;
}

// stat_t getDir(char *)
// {
//   // TODO:
// }
