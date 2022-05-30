#ifndef GAME_RECEIVE_H
#define GAME_RECEIVE_H

typedef enum type_message_game {
  UPMOV,
  DOMOV,
  LEMOV,
  RIMOV,
  IQUIT,
  GLIS,
  MALL,
  LEAD,
  SEND,
} type_message_game;

typedef struct command_game {
  int type;
  int size;
  char* data;
} command_game;

#include <Game/Labyrinth.h>
#include <Lobby/User.h>
#include <Server.h>
#include <Util.h>

command_game* receive_command_game(int sock);
void advance_user(user* user, type_message_game type, int value);
void checkGhost(int game_number, user* user);
int receive_upmov(user* user, char* buff);
int receive_domov(user* user, char* buff);
int receive_lemov(user* user, char* buff);
int receive_rimov(user* user, char* buff);
int receive_glis(user* user);
int receive_mall(user* user, char* buff);
int receive_send(user* user, char* buff);
int receive_lead(user* user_);
#endif