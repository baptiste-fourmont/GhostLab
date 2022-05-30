#ifndef LOBBY_RECEIVE_H
#define LOBBY_RECEIVE_H


typedef enum type_message_client {
  NEWPL,
  REGIS,
  UNREG,
  SIZEH,
  LISTH,
  GAMEH,
  START,
  SIZE,
} type_message_client;

typedef struct command_client {
  int type;
  int size;
  char* data;
} command_client;

#include <Game/Labyrinth.h>
#include <Lobby/User.h>
#include <Server.h>
#include <Util.h>

int receive_unreg(user* user);
int receive_newpl(user* user_to_add, char* buff);
int receive_regis(user* user_to_add, char* buffer);
int receive_game(int sock);
int receive_list(int sock, char* buff);
int receive_size(int sock, char *buff);
command_client* receive_command(int sock);
int is_valid_commmand(command_client* com);
#endif