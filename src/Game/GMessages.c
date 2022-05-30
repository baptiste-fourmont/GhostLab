#include "Game/GMessages.h"
#include <endian.h>

/**
 * @brief Permet d'envoyer le message et de rajouter ***
 *
 * @param sockfd
 * @param buffer
 * @param offset Taille exacte de là ou en est dans le buffer
 */
void send_game_message(int sockfd, char* buffer, int offset) {
  memmove(buffer + offset, "***", 3);
  if (send(sockfd, buffer, offset + 3, 0) == -1) {
    perror("ERROR : send_game_message");
    exit(EXIT_FAILURE);
  }
}

void send_gplyr(user* user, int sockfd) {
  char* name = user->name;
  int point = user->point;
  int x = user->x;
  int y = user->y;
  int offset = 0;

  char buffer[30];
  char* p_ = malloc(sizeof(char) * 5);
  if (p_ == NULL) {
    die("Error during malloc point in gplyr");
  }
  convert_to_point(p_, point);
  p_[4] = '\0';
  char* x_ = malloc(sizeof(char) * 4);
  if (x_ == NULL) {
    die("Error during malloc x in gplyr");
  }
  convert_to_pos(x_, x);
  x_[3] = '\0';
  char* y_ = malloc(sizeof(char) * 4);
  if (y_ == NULL) {
    die("Error during malloc y in gplyr");
  }
  convert_to_pos(y_, y);
  y_[3] = '\0';

  memmove(buffer + offset, "GPLYR", 5);
  offset += 5;
  memmove(buffer + offset, " ", 1);
  offset += 1;
  memmove(buffer + offset, name, 8);
  offset += 8;
  memmove(buffer + offset, " ", 1);
  offset += 1;
  memmove(buffer + offset, x_, 3);
  offset += 3;
  memmove(buffer + offset, " ", 1);
  offset += 1;
  memmove(buffer + offset, y_, 3);
  offset += 3;
  memmove(buffer + offset, " ", 1);
  offset += 1;
  memmove(buffer + offset, p_, 4);
  offset += 4;
  send_game_message(sockfd, buffer, offset);

  if (p_ != NULL) {
    free(p_);
  }
  if (x_ != NULL) {
    free(x_);
  }
  if (y_ != NULL) {
    free(y_);
  }
}

/**
 * @brief Le Serveur répond [GLIS! s***"
 * @param sockfd
 * @param s est codé sur 1 octets et contient le nombre d’inscrits à une partie
 */
void send_glis(user* user) {
  int offset = 0;
  int sockfd = user->sock;
  int party_ = user->game_number;
  uint8_t s = games_list[user->game_number]->connected_player_number;
  char buffer[10];
  memmove(buffer + offset, "GLIS!", 5);
  offset += 5;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, &s, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  send_game_message(sockfd, buffer, offset);

  int nb_player = (int)s;
  for (int i = 0; i < nb_player; i++) {
    pthread_mutex_lock(&mutex);
    send_gplyr(games_list[party_]->list[i], sockfd);
    pthread_mutex_unlock(&mutex);
  }
  return;
}

/**
 * @brief [MALL!***"
 *  Réponse du serveur lorsqu'un joueur envoie à tout le monde un message via
 * [MALL? messs***"
 * @param sockfd
 */
void send_mall(int sockfd) {
  int offset = 0;
  char buffer[8];
  memmove(buffer + offset, "MALL!", strlen("MALL!"));
  offset += strlen("MALL!");
  send_game_message(sockfd, buffer, offset);
  return;
}

/**
 * @brief [SEND!***]" si l'envoie du message est possible
 * Réponse du serveur pour  [SEND?␣id␣mess***"
 * @param sockfd
 */
void send_send(int sockfd) {
  int offset = 0;
  char buffer[8];
  memmove(buffer + offset, "SEND!", strlen("SEND!"));
  offset += strlen("SEND!");
  send_game_message(sockfd, buffer, offset);
  return;
}

/**
 * @brief Not send
 * [NSEND***]"
 * @param sockfd
 */
void send_nsend(int sockfd) {
  int offset = 0;
  char buffer[8];
  memmove(buffer + offset, "NSEND", strlen("NSEND"));
  offset += strlen("NSEND");
  send_game_message(sockfd, buffer, offset);
  return;
}

/**
 * @brief
 *
 * @param sockfd
 * @param m codé sur un octet
 * @param w codé sur deux
 * @param h
 * @param ip
 * @param port
 * @return int
 */
int send_welcome(int sockfd, uint8_t m, uint16_t h, uint16_t w, uint8_t f,
                 char* ip, char* port) {
  int offset = 0;
  char buffer[36 + 3];
  ip[15] = '\0';

  uint16_t w_ = htole16(w);
  uint16_t h_ = htole16(h);

  memmove(buffer + offset, "WELCO", 5);
  offset += 5;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, &m, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, &h_, sizeof(uint16_t));
  offset += sizeof(uint16_t);
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, &w_, sizeof(uint16_t));
  offset += sizeof(uint16_t);
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, &f, sizeof(uint8_t));
  offset += 1;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, ip, 15);
  offset += 15;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, port, 4);
  offset += 4;
  send_game_message(sockfd, buffer, offset);
  return 0;
}

void send_move_without_ghost(int sockfd, int x, int y) {
  int offset = 0;
  char buffer[16];

  char* x_ = malloc(sizeof(char) * 4);
  if (x_ == NULL) {
    die("Error during send_move_without_ghost malloc");
  }
  char* y_ = malloc(sizeof(char) * 4);
  if (y_ == NULL) {
    die("Error during send_move_without_ghost malloc");
  }

  convert_to_pos(x_, x);
  x_[3] = '\0';
  convert_to_pos(y_, y);
  y_[3] = '\0';

  memmove(buffer + offset, "MOVE!", 5);
  offset += 5;
  memmove(buffer + offset, " ", 1);
  offset += 1;
  memmove(buffer + offset, x_,  3);
  offset += 3;
  memmove(buffer + offset, " ", 1);
  offset += 1;
  memmove(buffer + offset, y_, 3);
  offset += 3;
  send_game_message(sockfd, buffer, offset);

  if (x_ != NULL) {
    free(x_);
  }
  if (y_ != NULL) {
    free(y_);
  }
}

void send_move_with_ghost(user* user, int x, int y) {
  int offset = 0;
  char buffer[21];
  int sockfd = user->sock;
  int point = user->point;

  char* x_ = malloc(sizeof(char) * 4);
  if (x_ == NULL) {
    die("Error during send_move_with_ghost malloc");
  }
  char* y_ = malloc(sizeof(char) * 4);
  if (y_ == NULL) {
    die("Error during send_move_with_ghost malloc");
  }
  char* p_ = malloc(sizeof(char) * 5);
  if (p_ == NULL) {
    die("Error during send_move_with_ghost malloc");
  }

  convert_to_pos(x_, x);
  x_[3] = '\0';
  convert_to_pos(y_, y);
  y_[3] = '\0';
  convert_to_point(p_, point);
  p_[4] = '\0';

  memmove(buffer + offset, "MOVEF", 5);
  offset += 5;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, x_, 3);
  offset += 3;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, y_, 3);
  offset += 3;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, p_, 4);
  offset += 4;
  send_game_message(sockfd, buffer, offset);

  if (x_ != NULL) {
    free(x_);
  }
  if (y_ != NULL) {
    free(y_);
  }
  if(p_ != NULL) {
    free(p_);
  }
  return;
}

void send_posit(user* user, int x, int y) {
  int offset = 0;
  char buffer[25];

  char* x_ = malloc(sizeof(char) * 4);
  if (x_ == NULL) {
    die("Error during send_posit malloc");
  }
  char* y_ = malloc(sizeof(char) * 4);
  if (y_ == NULL) {
    die("Error during send_posit malloc");
  }
  convert_to_pos(x_, x);
  x_[3] = '\0';
  convert_to_pos(y_, y);
  y_[3] = '\0';

  memmove(buffer + offset, "POSIT", 5);
  offset += 5;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, user->name, sizeof(char) * 8);
  offset += 8;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, x_, 3);
  offset += 3;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, y_, 3);
  offset += 3;
  send_game_message(user->sock, buffer, offset);

  if (x_ != NULL) {
    free(x_);
  }
  if (y_ != NULL) {
    free(y_);
  }
  return;
}

/**
 * Fonction qui permet d'envoyer GODBYE
 * @param sockfd
 * @return: [GODBYE***]"
 */
void send_gobye(int sockfd) {
  int offset = 0;
  char buffer[8];
  memmove(buffer + offset, "GOBYE", 5);
  offset += 5;
  send_game_message(sockfd, buffer, offset);
  return;
}