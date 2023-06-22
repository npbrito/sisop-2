#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <errno.h>
#include "error.h"
#include "dir.h"

pthread_mutex_t mutex;

user_t createUserDir(const char *username)
{
  fprintf(stdout, "createUserDir %s\n", username);
  user_t user;
  user.username = strdup(username);
  user.dir = NULL;

  int n = mkdir(username, 0700);

  if (n == 0)
  {
    user.dir = strdup(username);
  }
  else
  {
    err_sys("createUserDir error");
  }

  return user;
}

bool checkDirExists(const char *dir)
{
  fprintf(stdout, "checkDirExists %s\n", dir);

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

// stat_t getDir(const char *)
// {
//   // TODO:
// }
