#include "Server.h"

#include <pthread.h>
pthread_mutex_t mutex;
int count_game;
int count_port;
game** games_list;

typedef struct thread_params {
  user* user;
  int sock;
  int mode;
} thread_params;

void* handle_client(void* args) {
  thread_params* params = (thread_params*)args;
  user* user = params->user;
  int sock = user->sock;
  int mode = params->mode;

  free(args);

  uint8_t games_in_waiting = getGamesInWaiting();
  send_games(sock, games_in_waiting);

  if (interact_lobby(user, mode) == 1) {
    if (user != NULL && user->name != NULL) {

      pthread_mutex_lock(&mutex);
      delete_user(user, 0);
      pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
    return NULL;
  }

  /**
   * On veut vérifier que si c'est le dernier à lancer la partie
   * Alors la partie est lancée si ca ne fonctionne pas
   */

  // TODO: URGENT!
  // Il faut qu'on vérifie qu'on recoit un message de type "WELCOME"

  while (checkStart(user->game_number) == 0) {
    sleep(1);
  }
  // Sinon elle est stocké dans le labyrinthe
  uint8_t m = user->game_number;
  pthread_mutex_lock(&mutex);
  char* ip = games_list[m]->ip;
  char* port = games_list[m]->port;
  uint16_t w = games_list[m]->lab->columns;
  uint16_t h = games_list[m]->lab->lines;
  uint8_t f = games_list[m]->lab->nb_ghost;
  pthread_mutex_unlock(&mutex);

  send_welcome(sock, m, h, w, f, ip, port);

  pthread_mutex_lock(&mutex);
  send_posit(user, user->x, user->y);
  pthread_mutex_unlock(&mutex);

  if (interact_game(user, mode) == 1) {

    pthread_mutex_lock(&mutex);
    delete_user(user, 1);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
  }

  pthread_exit(NULL);
  return NULL;
}

/**
 *
 * @param sock
 * @return int
 */

int interact_lobby(user* user, int mode) {
  int is_register = 0;
  int sock = user->sock;
  int res;
  while (1) {
    command_client* command = malloc(sizeof(command_client));
    memset(command, 0, sizeof(command_client));
    command = receive_command(sock);
    if (command == NULL) {
      return 1;
    }
    command->data[command->size] = '\0';
    if (mode == 1) {
      printf("Commande reçu : '%s'\n", command->data);
    }
    switch (command->type) {
    case NEWPL:
      if (is_register == 1) {
        send_regno(sock);
      } else {
        res = receive_newpl(user, command->data);
        if (res == 1) {
          send_regno(sock);
        } else {
          is_register = 1;
        }
        pthread_mutex_lock(&mutex);
        print_labyrinth(games_list[user->game_number]->lab);
        pthread_mutex_unlock(&mutex);
      }
      break;
    case LISTH:
      receive_list(sock, command->data);
      break;
    case GAMEH:
      res = receive_game(sock);
      break;
    case REGIS:
      if (is_register == 1) {
        send_regno(sock);
      } else {
        res = receive_regis(user, command->data);
        if (res == 0) {
          is_register = 1;
        }
      }
      break;
    case UNREG:
      res = receive_unreg(user);
      is_register = 0;
      break;
    case START:
      if (is_register == 1) {
        pthread_mutex_lock(&mutex);
        set_user_start(user);
        pthread_mutex_unlock(&mutex);
        free(command->data);
        free(command);
        return 2;
      } else {
        send_dunno(sock);
      }
      break;
    case SIZE:
      res = receive_size(sock, command->data);
      break;
    default:
      send_dunno(sock);
      break;
    }
    free(command->data);
    free(command);
  }
  return 0;
}

int interact_game(user* user_, int mode) {
  int sock = user_->sock;
  while (1) {
    command_game* command = malloc(sizeof(command_game));
    command = receive_command_game(sock);
    if (command == NULL) {
      return 1;
    }
    command->data[command->size] = '\0';
    if (mode == 1) {
      printf("Commande reçu : '%s'\n", command->data);
    }
    // On envoie GOBYE si la partie est fini
    switch (command->type) {
    case IQUIT: {
      printf("%s a quitté la partie\n", user_->name);
      pthread_mutex_lock(&mutex);

      send_gobye(sock);
      labyrinth* laby = games_list[user_->game_number]->lab;
      for (int i = 0; i < laby->lines; i++) {
        for (int j = 0; j < laby->columns; j++) {
          if (laby->tab[i][j].user != NULL &&
              laby->tab[i][j].user->sock == user_->sock) {
            laby->tab[i][j].user = NULL;
            laby->tab[i][j].status = NONE;
          }
        }
      }

      int game_number = user_->game_number;
      if (user_ != NULL) {
        delete_user(user_, 0);
      }

      if (games_list[game_number]->connected_player_number == 1) {
        user* win = NULL;
        if (games_list[game_number]->connected_player_number == 1) {
          win = games_list[game_number]->list[0];
        } else {
          for (int i = 0; i < games_list[game_number]->connected_player_number;
               i++) {
            if (games_list[game_number]->list[i]->point >= win->point) {
              win = games_list[game_number]->list[i];
            }
          }
        }

        if (games_list[game_number]->connected_player_number == 1) {
          send_gobye(win->sock);
          char* msg = send_enga(win->name, win->point);
          create_sender(atoi(games_list[game_number]->port), msg);
          if (msg != NULL) {
            free(msg);
          }
          delete_user(win, 0);
        }
      }
      pthread_mutex_unlock(&mutex);
    } break;
    case DOMOV:
      receive_domov(user_, command->data);
      pthread_mutex_lock(&mutex);
      checkGhost(user_->game_number, user_);
      pthread_mutex_unlock(&mutex);
      break;
    case RIMOV:
      receive_rimov(user_, command->data);
      pthread_mutex_lock(&mutex);
      checkGhost(user_->game_number, user_);
      pthread_mutex_unlock(&mutex);
      break;
    case LEMOV:
      receive_lemov(user_, command->data);
      pthread_mutex_lock(&mutex);
      checkGhost(user_->game_number, user_);
      pthread_mutex_unlock(&mutex);
      break;
    case UPMOV: {
      receive_upmov(user_, command->data);
      pthread_mutex_lock(&mutex);
      checkGhost(user_->game_number, user_);
      pthread_mutex_unlock(&mutex);
      break;
    }
    case MALL:
      receive_mall(user_, command->data);
      break;
    case GLIS:
      receive_glis(user_);
      break;
    case SEND:
      receive_send(user_, command->data);
      break;
    case LEAD:
      receive_lead(user_);
      break;
    default:
      send_dunno(sock);
      break;
    }
    free(command->data);
    free(command);
  }
  return 0;
}

int main(int argc, char** argv) {
  srand(time(NULL));
  if (argc != 3) {
    perror("./server [PORT] [mode(0/1)]");
    exit(EXIT_FAILURE);
  }

  if (atoi(argv[2]) < 0 || atoi(argv[2]) > 1) {
    perror("./server [PORT] [mode(0/1)]");
    exit(EXIT_FAILURE);
  }

  pthread_mutex_init(&mutex, PTHREAD_MUTEX_DEFAULT);
  int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
  if (serv_sock < 0) {
    perror("socket");
    exit(1);
  }

  struct sockaddr_in serv_addr;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(atoi(argv[1]));

  if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("bind");
    exit(1);
  }

  if (listen(serv_sock, 0) < 0) {
    perror("listen");
    exit(1);
  }

  count_game = 0;
  count_port = 4446;

  while (1) {
    struct sockaddr_in cli_addr;
    socklen_t cli_addr_len = sizeof(cli_addr);
    int* cli_sock = malloc(sizeof(int));
    if (cli_sock == NULL) {
      perror("ERROR : malloc of create client sock");
      exit(EXIT_FAILURE);
    }
    *cli_sock = accept(serv_sock, (struct sockaddr*)&cli_addr, &cli_addr_len);

    if (*cli_sock < 0) {
      perror("accept");
      exit(1);
    }

    pthread_t thread;
    thread_params* params = malloc(sizeof(thread_params));
    params->user = malloc(sizeof(user));
    params->user->sock = *cli_sock;
    params->sock = *cli_sock;
    params->mode = atoi(argv[2]);

    free(cli_sock);
    if (pthread_create(&thread, NULL, handle_client, params) != 0) {
      perror("pthread_create");
      exit(EXIT_FAILURE);
    }
  }
  // TODO: Free ici tout la structure
  if (games_list != NULL) {
    delete_all_games();
  }

  pthread_mutex_destroy(&mutex);
  pthread_exit(NULL);
  return 0;
}