#include <Lobby/Game.h>

game* create_game() {
  game* new_game = malloc(sizeof(game));
  if (new_game == NULL) {
    die("ERROR : malloc game in create_game");
  }
  user** list = malloc(sizeof(user*) * MAX_IN_GAME);
  if (list == NULL) {
    die("ERROR : malloc list in create_game");
  }
  char* ip = malloc(sizeof(char) * 16);
  if (ip == NULL) {
    die("ERROR : malloc ip in create_game");
  }
  sprintf(ip, "%s", "224.0.0.251");

  char* port = malloc(sizeof(char) * 5);
  if (port == NULL) {
    die("ERROR : malloc port in create_game");
  }
  pthread_mutex_lock(&mutex);
  sprintf(port, "%d", count_port);
  count_port++;
  pthread_mutex_unlock(&mutex);

  printf("IP : %s, PORT: %d\n", ip, atoi(port));
  char *ip_ = complete_ip(ip);

  labyrinth* laby = create_labyrinth(12, 12, 2);
  laby->game_port = atoi(port);

  new_game->list = list;
  new_game->connected_player_number = 1;
  new_game->statut = STATUS_PASCOMMENCE;
  new_game->ip = ip_;
  new_game->port = port;
  new_game->lab = laby;
  return new_game;
}

uint8_t getGamesInWaiting() {
  uint8_t count = 0;

  pthread_mutex_lock(&mutex);
  int number_of_games = count_game;
  game** game = games_list;

  for (int i = 0; i < number_of_games; i++) {
    if (game[i]->statut == STATUS_PASCOMMENCE) {
      count++;
    }
  }
  pthread_mutex_unlock(&mutex);
  return count;
}

int getNumeroOfParty(int sockfd) {
  for (int i = 0; i < count_game; i++) {
    for (int j = 0; j < games_list[i]->connected_player_number; j++) {
      if (games_list[i]->list[j]->sock == sockfd) {
        return i;
      }
    }
  }
  return -1;
}

void delete_game(game* game) {
  free(game->ip);
  free(game->port);
  for (int i = 0; i < game->connected_player_number; i++) {
    free(game->list[i]);
  }
  free(game->list);
  delete_labyrinth(game->lab);
  free(game);
}

/**
 * @brief Delete all games for free it
 *
 */
void delete_all_games() {
  for (int i = 0; i < count_game; i++) {
    for (int j = 0; j < games_list[i]->connected_player_number; j++) {
      if (games_list[i]->list[j] != NULL) {
        free(games_list[i]->list[j]->name);
        free(games_list[i]->list[j]->udp_port);
        free(games_list[i]->list[j]);
      }
    }
    if (games_list[i] != NULL) {
      free(games_list[i]->list);
      free(games_list[i]);
    }
  }
  if (games_list != NULL) {
    free(games_list);
  }
}