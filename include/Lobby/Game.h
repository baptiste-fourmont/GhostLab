#ifndef GAME_H
#define GAME_H

#define MAX_NAME 8
#define MAX_IN_GAME 5

#include <Game/Labyrinth.h>
#include <Lobby/User.h>
#include <Server.h>
#include <Util.h>

/**
 * @brief start: 0 permet de dire qu'il n'a pas envoyé start
 * 1 permet de dire qu'il a envoyé start
 *
 */

enum status { STATUS_PASCOMMENCE = 0, STATUS_ENCOURS = 1, STATUS_FINI = 2 };

typedef struct game {
  int game_number;
  int connected_player_number;
  int statut;
  user** list;
  char* ip;
  char* port;
  labyrinth* lab;
} game;

game* create_game();
uint8_t getGamesInWaiting();
int getNumeroOfParty(int sockfd);
void delete_game(game* game);
void delete_all_games();

extern game** games_list;
extern int count_game;
extern int count_port;

#endif