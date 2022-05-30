#ifndef USER_H
#define USER_H

#include <Util.h>

typedef struct user {
  char* name;
  char* udp_port;
  int sock;
  int start;
  int game_number;
  int point;
  uint16_t x;
  uint16_t y;
} user;

struct user_save {
  char name[8];
  int point;
};

int username_taken(char* username);
int create_user(user* user, char* buffer);
user* find_user(char* id);
int find_game_number(user* user);
int delete_user(user* user_to_delete, int is_receive);
int count_start(int gameNumber);
int set_user_start(user* user);
int checkStart(int gameNumber);
int need_toStart(int sock);

#endif