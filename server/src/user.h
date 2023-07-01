#ifndef USER_H
#define USER_H

typedef struct user
{
  char *username;
  char *dir;
} user_t;

user_t recv_user(int sockfd);
void get_sync_dir(user_t user);

#endif