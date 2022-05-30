#include "Game/Multi.h"

/**
 * @brief Permet de voir si le message ne contient pas +++ ou ****
 *
 * @param msg
 * @return int
 */

void send_udp(const char* msg, int port) {
  int sockfd;
  struct sockaddr_in servaddr;

  // Creating socket file descriptor
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    die("socket creation failed");
  }

  memset(&servaddr, 0, sizeof(servaddr));

  // Filling server information
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  servaddr.sin_addr.s_addr = INADDR_ANY;

  printf("SEND UDP %s", msg);

  sendto(sockfd, msg, strlen(msg), MSG_CONFIRM,
         (const struct sockaddr*)&servaddr, sizeof(servaddr));
}

void send_multi(int sockfd, char* buffer, int offset, struct sockaddr* saddr) {
  memmove(buffer + offset, "+++", 3);
  if (sendto(sockfd, buffer, offset + 3, 0, saddr,
             (socklen_t)sizeof(struct sockaddr_in)) == -1) {
    die("sendto");
  }
}

/**
 * @brief
 *
 * @param user
 * @param id
 * @param msg
 * @return int
 */
int send_messp(user* ask, char* id, char* msg) {
  printf("MESSAGE DANS messp: %s", msg);
  char buffer[5+1+8+1+strlen(msg)+3];
  int offset = 0;
  if (strlen(id) != 8 || strlen(msg) <= 0) {
    send_nsend(ask->sock);
    return 0;
  }
  user* player = find_user(id);
  if (player == NULL || ask->game_number != player->game_number) {
    send_nsend(ask->sock);
    return 0;
  }

  memmove(buffer + offset, "MESSP", 5);
  offset += 5;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  ask->name[8] = '\0';
  memmove(buffer + offset, ask->name, 8);
  offset += 8;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, msg, strlen(msg));
  offset += strlen(msg);
  memmove(buffer + offset, "+++", 3);
  printf("PRINT AVANT ENVOIE MESSP %s", buffer);
  send_send(ask->sock);
  send_udp(buffer, atoi(player->udp_port));
  return 0;
}

/*
 * @brief
 *
 * @param sockfd
 * @param saddr
 * @param id 8 caractères alpha numériques
 * @param msg
 */

char* send_messa(char* id, char* msg) {
  int offset = 0;
  char* buffer = malloc(5 + 1 + 8 + 1 + strlen(msg) + 3);
  if (buffer == NULL) {
    die("ERROR : malloc buffer in send_messa");
  }
  memmove(buffer + offset, "MESSA", 5);
  offset += 5;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, id, 8);
  offset += 8;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, msg, strlen(msg));
  offset += strlen(msg);
  memmove(buffer + offset, "+++", 3);
  offset += 3;
  printf("%d\n", offset);
  return buffer;
}

/* @brief La partie multidiffuse [GHOST␣x␣y+++]
 * FIXME: Doit on cnvertir x et y dans la fonction ou en dehors
 * @param sockfd
 * @param saddr
 * @param x est un numéro de ligne codé sur trois octets.
 * @param y est un numéro de ligne codé sur trois octets.
 */

char* send_ghost(int x, int y) {
  int offset = 0;
  char* buffer = malloc(sizeof(char) * 16);
  if (buffer == NULL) {
    die("malloc send_ghost");
  }

  char* x_ = malloc(sizeof(char) * 4);
  if (x_ == NULL) {
    die("Errror in send_ghost malloc");
  }
  x_[3] = '\0';
  char* y_ = malloc(sizeof(char) * 4);
  if (y_ == NULL) {
    die("Errror in send_ghost malloc");
  }
  y_[3] = '\0';
  convert_to_pos(x_, x);
  convert_to_pos(y_, y);

  memmove(buffer + offset, "GHOST", 5);
  offset += 5;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, x_, 3);
  offset += 3;
  memmove(buffer + offset, " ", 1);
  offset += 1;
  memmove(buffer + offset, y_, 3);
  offset += 3;
  memmove(buffer + offset, "+++", 3);
  return buffer;
}

/* @brief Le serveur multi-diffuse [ENDGA␣id␣p+++]
 * @param sockfd
 * @param saddr
 * @param id 8 caractères alpha numériques
 * @param p est un nombre de points codé sur quatre octets. I
 */

char* send_enga(char* id, int p) {
  char* buffer = malloc(sizeof(char) * 22);
  if (buffer == NULL) {
    die("Errror in send_enga malloc");
  }
  int offset = 0;
  char* p_ = malloc(sizeof(char) * 5);
  p_[4] = '\0';
  convert_to_point(p_, p);

  memmove(buffer, "ENDGA", 5);
  offset += 5;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, id, 8);
  offset += 8;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, p_, 4);
  offset += 4;
  memmove(buffer + offset, "+++", 3);
  // send_multi(sockfd, buffer, offset, saddr);
  return buffer;
}

char* send_score(char* id, int p, int x, int y) {
  int offset = 0;
  char* buffer = malloc(sizeof(char) * 30);

  char* p_ = malloc(sizeof(char) * 5);
  if (p_ == NULL) {
    die("Error in send_score malloc");
  }
  p_[4] = '\0';
  char* x_ = malloc(sizeof(char) * 4);
  if (x_ == NULL) {
    die("Errror in send_score malloc");
  }
  x_[3] = '\0';
  char* y_ = malloc(sizeof(char) * 4);
  if (y_ == NULL) {
    die("Errror in send_score malloc");
  }
  y_[3] = '\0';
  convert_to_pos(x_, x);
  convert_to_pos(y_, y);
  convert_to_point(p_, p);

  memmove(buffer, "SCORE", 5);
  offset += 5;
  memmove(buffer + offset, " ", 1);
  offset += 1;
  memmove(buffer + offset, id, 8);
  offset += 8;
  memmove(buffer + offset, " ", 1);
  offset += 1;
  memmove(buffer + offset, p_, 4);
  offset += 4;
  memmove(buffer + offset, " ", 1);
  offset += 1;
  memmove(buffer + offset, x_, 3);
  offset += 3;
  memmove(buffer + offset, " ", 1);
  offset += 1;
  memmove(buffer + offset, y_, 3);
  offset += 3;
  memmove(buffer + offset, "+++", 3);
  return buffer;
}