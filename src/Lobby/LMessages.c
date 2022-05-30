#include "Lobby/LMessages.h"

/**
 * @brief Permet d'envoyer le message et de rajouter ***
 *
 * @param sockfd
 * @param buffer
 * @param offset Taille exacte de là ou en est dans le buffer
 */
void send_lobby_message(int sockfd, char* buffer, int offset) {
  // on le changera quand on aura mis une lib socket
  memmove(buffer + offset, "***", 3);
  if (send(sockfd, buffer, offset + 3, 0) == -1) {
    perror("ERROR : send_lobby_message");
    exit(EXIT_FAILURE);
  }
}
/**
 * Fonction qui permet d'envoyer START
 * @param sockfd
 * @return: [START***"
 */
void send_start(int sockfd) {
  int offset = 0;
  char buffer[8];
  memmove(buffer + offset, "START", 5);
  offset += 5;
  send_lobby_message(sockfd, buffer, offset);
  return;
}

/**
 * @brief Fonction qui permet d'envoyer [OGAME m s***"
 *
 * @param sockfd
 * @param m indique le numéro de la partie et s indique le nombre de joueurs
 * pour la partie (1 octet)
 * @param s t contient le nombre d’inscrits à une partie (1 octet)
 */
void send_ogames(int sockfd, uint8_t m, uint8_t s) {
  int offset = 0;
  char buffer[12];
  memmove(buffer + offset, "OGAME", 5);
  offset += 5;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, &m, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, &s, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  send_lobby_message(sockfd, buffer, offset);
  return;
}

/**
 * @brief [GAMES n***"
 *
 * @param: n indique le nombre de partie avec des joueurs inscrit et non
 * commencé
 */
void send_games(int sockfd, uint8_t n) {
  int offset = 0;
  char buffer[10];

  memmove(buffer + offset, "GAMES", 5);
  offset += 5;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, &n, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  send_lobby_message(sockfd, buffer, offset);

  if (n > 0) {
    for (int i = 0; i < n; i++) {
      pthread_mutex_lock(&mutex);
      if (games_list[i]->statut == STATUS_PASCOMMENCE){
        send_ogames(sockfd, i, games_list[i]->connected_player_number);
      }
      pthread_mutex_unlock(&mutex);
    }
  }
}

/**
 * @brief Fonction qui permet de dire que le joueur a bien rejoint la partie m
 * @param m indique le numéro de la partie rejoint (1 octet)
 * @return:  [REGOK m***"
 */
void send_regok(int sockfd, uint8_t m) {
  int offset = 0;
  char buffer[10];
  memmove(buffer + offset, "REGOK", 5);
  offset += 5;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, &m, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  send_lobby_message(sockfd, buffer, offset);
  return;
}

/**
 * Fonction qui permet de dire que le joueur n'a pas rejoint la partie
 * @param sockfd
 * @return:  [REGNO***"
 */
void send_regno(int sockfd) {
  int offset = 0;
  char buffer[8];
  memmove(buffer + offset, "REGNO", 5);
  offset += 5;
  send_lobby_message(sockfd, buffer, offset);
  return;
}

/**
 * Fonction qui permet de dire que le joueur s'est désinscrit de la partie m
 * @param: m indique le numéro de la partie a se désinscrire (1 octet)
 * @return:  [REGOK m***"
 */
void send_unrok(int sockfd, uint8_t m) {
  int offset = 0;
  char buffer[10];
  memmove(buffer + offset, "UNROK", 5);
  offset += 5;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, &m, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  send_lobby_message(sockfd, buffer, offset);
  return;
}

/**
 * Fonction qui permet de dire que le joueur n'est pas inscrit à une partie
 * alors qu'il veut se désinscrire
 * @param sockfd
 * @return:  [DUNNO***"
 */
void send_dunno(int sockfd) {
  int offset = 0;
  char buffer[8];
  memmove(buffer + offset, "DUNNO", 5);
  offset += 5;
  send_lobby_message(sockfd, buffer, offset);
  return;
}

/**
 * @brief Réponse du Serveur: [SIZE! m h w***"
 *
 * @param sockfd
 * @param m est codé sur 1 octet et contient le nombre de partie
 * @param w est codée sur 2 octets en little endiant et contient la hauteur du
 * labyrinthe
 * @param h h est codé sur 2 octets en little endian et contient la hauteur d’un
 * labyrinthe
 */
void send_size(int sockfd, uint8_t m, uint16_t w, uint16_t h) {
  int offset = 0;
  char buffer[16];
  uint16_t weight = w;
  uint16_t height = h;

  weight = htole16(weight);
  height = htole16(height);

  memmove(buffer + offset, "SIZE!", 5);
  offset += 5;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, &m, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, &weight, sizeof(uint16_t));
  offset += sizeof(uint16_t);
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, &height, sizeof(uint16_t));
  offset += sizeof(uint16_t);
  send_lobby_message(sockfd, buffer, offset);
  return;
}

/**
 * @brief  Le serveur répond: [LIST!␣m␣s***"
 * @param sockfd
 * @param m est le nombre de joueurs inscrit à cette partie
 * @param s suivi de s message de la forme [PLAYR id***"
 */
void send_list(int sockfd, uint8_t m, uint8_t s) {
  int offset = 0;
  char buffer[12];
  memmove(buffer + offset, "LIST!", 5);
  offset += 5;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, &m, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, &s, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  send_lobby_message(sockfd, buffer, offset);

  int party = m;
  int player = (int)s;

  if (games_list[party] != NULL) {
    for (int i = 0; i < player; i++) {
      if (games_list[party]->list[i] != NULL) {
        send_player(sockfd, games_list[party]->list[i]->name);
      }
    }
  }
  return;
}

/**
 * @brief Le serveur envoie [PLAYR id***]
 *  Permet d'envoyer l'identité d'un joueur inscrit
 * @param sockfd
 * @param id id d'un utilisateur, il s'agit d'une chaine de caractère formée
 * d'exactement 8 caractères numériques
 */
void send_player(int sockfd, char* id) {
  if (strlen(id) != 8) {
    perror("ERROR : id is not 8 characters in send_player");
    return;
  }

  int offset = 0;
  char buffer[6 + 8 + 3];
  memmove(buffer + offset, "PLAYR", 5);
  offset += 5;
  memmove(buffer + offset, " ", sizeof(char));
  offset += 1;
  memmove(buffer + offset, id, sizeof(char) * 8);
  offset += 8;
  send_lobby_message(sockfd, buffer, offset);
  return;
}