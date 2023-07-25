#ifndef USER_H
#define USER_H

typedef struct user
{
  char *username;
  char *dir;
} user_t;

user_t save_user(char *username);
void get_sync_dir(char *userdir, int sockfd);
void initialize(user_t user, int sockfd);

#endif