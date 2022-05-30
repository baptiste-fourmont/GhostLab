#include "Lobby/User.h"

#include <pthread.h>

#include "Lobby/Game.h"
#include "Util.h"

int username_taken(char* username) {
  int number_of_games = count_game;
  game** game = games_list;

  for (int i = 0; i < number_of_games; i++) {
    for (int j = 0; j < game[i]->connected_player_number; j++) {
      if (strcmp(username, game[i]->list[j]->name) == 0) {
        return 1;
      }
    }
  }
  return 0;
}

int create_user(user* user, char* buffer) {
  int offset = 6;
  user->name = malloc(sizeof(char) * 9);
  if (user->name == NULL) {
    die("error name in create_user");
  }
  user->udp_port = malloc(sizeof(char) * 5);
  if (user->udp_port == NULL) {
    die("error udp_port in create_user");
  }
  user->start = 0;

  memmove(user->name, buffer + offset, 8);
  user->name[8] = '\0';

  pthread_mutex_lock(&mutex);
  int res = username_taken(user->name);
  pthread_mutex_unlock(&mutex);
  if (res == 1) {
    return 1;
  }
  offset += 9;
  memmove(user->udp_port, buffer + offset, 4);
  user->udp_port[4] = '\0';
  return 0;
}

user* find_user(char* id) {
  user* user_to_find = NULL;

  int number_of_games = count_game;
  game** game = games_list;

  for (int i = 0; i < number_of_games; i++) {
    for (int j = 0; j < game[i]->connected_player_number; j++) {
      if (strcmp(game[i]->list[j]->name, id) == 0) {
        user_to_find = game[i]->list[j];
        break;
      }
    }
  }
  return user_to_find;
}

int find_game_number(user* user) {
  int game_number = -1;

  int number_of_games = count_game;
  game** game = games_list;

  for (int i = 0; i < number_of_games; i++) {
    for (int j = 0; j < game[i]->connected_player_number; j++) {
      if (strcmp(game[i]->list[j]->name, user->name) == 0) {
        game_number = i;
        break;
      }
    }
  }
  return game_number;
}

int delete_user(user* user_to_delete, int is_receive) {
  game** game = games_list;
  int number_of_games = count_game;

  int sock = user_to_delete->sock;
  if (number_of_games == 0 || user_to_delete == NULL ||
      user_to_delete->sock == -1 || game == NULL || game[0] == NULL) {
    return 1;
  }
  for (int i = 0; i < number_of_games; i++) {
    for (int j = 0; j < game[i]->connected_player_number; j++) {
      if (strcmp(game[i]->list[j]->name, user_to_delete->name) == 0) {
        if (user_to_delete == NULL) {
          return 1;
        }
        if (user_to_delete->name != NULL) {
          free(user_to_delete->name);
        }
        if (user_to_delete->udp_port != NULL) {
          free(user_to_delete->udp_port);
        }

        for (int k = j; k < game[i]->connected_player_number; k++) {
          game[i]->list[k]->game_number = k + 1;
          game[i]->list[k] = game[i]->list[k + 1];
        }

        game[i]->connected_player_number--;
        if (game[i]->connected_player_number == 0) {
          if (game[i]->list != NULL) {
            free(game[i]->list);
          }
          if (game[i] != NULL) {
            free(game[i]);
            count_game--;
          }
          for (int k = i; k < count_game; k++) {
            game[k] = game[k + 1];
            for (int l = i; l < game[i]->connected_player_number; l++) {
              game[k]->list[l]->game_number = k;
            }
          }
        }
        if (is_receive == 1) {
          send_unrok(sock, (uint8_t)i);
        }

        return 0;
      }
    }
  }
  return 1;
}

int count_start(int game_id) {
  int count = 0;
  for (int i = 0; i < games_list[game_id]->connected_player_number; i++) {
    if (games_list[game_id]->list[i]->start == 1) {
      count++;
    }
  }
  return count;
}

/**
 * TODO: Not use yet and don't know if it works
 * @brief Permet de dire si il y a un utilisateur
 *
 * @param name identifiant de l'utilisateur
 * @return int
 * if start return 1
 * else return 0
 */
int set_user_start(user* user) {
  int sockfd = user->sock;
  int gameNumber = user->game_number;
  int number_of_games = count_game;
  game** game = games_list;

  for (int i = 0; i < number_of_games; i++) {
    for (int j = 0; j < game[i]->connected_player_number; j++) {
      if (game[i]->list[j]->sock == sockfd) {
        game[i]->list[j]->start = 1;
        if (count_start(gameNumber) ==
            game[gameNumber]->connected_player_number) {
          games_list[gameNumber]->statut = STATUS_ENCOURS;
          labyrinth* laby = games_list[gameNumber]->lab;

          int nb_player_set = 0;
          int nb_player = games_list[gameNumber]->connected_player_number;
          while(nb_player_set < nb_player) {
            int x = rand() % laby->lines;
            if (x == 0) {
              x = 1;
            }
            if (x == laby->lines) {
              x = laby->lines - 1;
            }
            int y = rand() % laby->columns;
            if (y == 0) {
              y = 1;
            }
            if (y == laby->columns) {
              y = laby->columns - 1;
            }
            if (laby->tab[x][y].status == NONE) {
              laby->tab[x][y].status = PLAYER;
              laby->tab[x][y].user =
                  games_list[gameNumber]->list[nb_player_set];
              games_list[gameNumber]->list[nb_player_set]->x = x;
              games_list[gameNumber]->list[nb_player_set]->y = y;
              printf("%d %d\n", x, y);
              nb_player_set++;
            }
          }
        }
        return 1;
      }
    }
  }
  return 0;
}

/**
 * @brief Get Party
 * -1 if error
 */
int checkStart(int gameNumber) {
  pthread_mutex_lock(&mutex);
  for (int i = 0; i < games_list[gameNumber]->connected_player_number; i++) {
    if (games_list[gameNumber]->list[i]->start != 1) {
      pthread_mutex_unlock(&mutex);
      return 0;
    }
  }
  pthread_mutex_unlock(&mutex);
  return 1;
}

/**
 * @brief need_toStart
 * If return 1 is needed if 0 is not needed
 */
int need_toStart(int sock) {
  int numero = getNumeroOfParty(sock);
  for (int j = 0; j < games_list[numero]->connected_player_number; j++) {
    if (games_list[numero]->list[j]->start != 1) {
      return 0;
    }
  }

  return 1;
}