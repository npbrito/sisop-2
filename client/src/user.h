#ifndef USER_H
#define USER_H

typedef struct user
{
  char *username;
  char *dir;
} user_t;

user_t save_user(char * username);
void setup_user(user_t user);

#endif