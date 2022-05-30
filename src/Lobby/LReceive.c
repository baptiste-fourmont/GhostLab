#include <Lobby/LReceive.h>

int receive_unreg(user* user) {
  int res;
  int number;
  int sock = user->sock;

  number = user->game_number;
  if (number == -1 || user->name == NULL) {
    send_dunno(sock);
    return 1;
  }

  pthread_mutex_lock(&mutex);
  res = delete_user(user, 1);
  printf("Delete user");
  pthread_mutex_unlock(&mutex);

  if (res == 1) {
    send_dunno(sock);
    return 1;
  }
  return 0;
}

/**
 * @brief Check if message is a command
 * RETURN: 1 if it is a command
 */
int is_valid_commmand(command_client* com) {
  char* data = com->data;
  int type = com->type;
  int size = com->size;

  if (size < 8) {
    return 0;
  }

  switch (type) {
    case START:
    case UNREG:
    case GAMEH:
      if (size != 8) {
        return 0;
      }
      break;
    case NEWPL:
      if (size != 22) {
        return 0;
      }
      for (int i = 6; i < 14; i++) {
        if (!isalnum(data[i])) {
          return 0;
        }
      }
      for (int i = 15; i < 19; i++) {
        if (isalpha(data[i])) {
          return 0;
        }
      }
      if (data[5] != ' ' || data[14] != ' ') {
        return 0;
      }
      break;
    case REGIS:
      if (size != 25) {
        printf("size : %d\n", size);
        return 0;
      }
      for (int i = 6; i < 14; i++) {
        if (!isalnum(data[i])) {
          printf("alnum : %c\n", data[i]);
          return 0;
        }
      }
      for (int i = 15; i < 19; i++) {
        if (isalpha(data[i])) {
          printf("alpha : %c\n", data[i]);
          return 0;
        }
      }
      if (data[5] != ' ' || data[14] != ' ' || data[19] != ' ') {
        printf("data[5] != ' ' || data[14] != ' ' || data[19] != ' '\n");
        return 0;
      }
      break;
    case LISTH:
    case SIZEH:
      if (size != 11 || data[5] != ' ') {
        return 0;
      }
      break;
    default:
      break;
  }
  return 1;
}

/**START
 * @brief
 * TODO: Trouver pour PORT et nom d'utilisateur plus grand
 * @brief Demande du joueur pour créer une partie sur un PORT
 * [NEWPL id port***]
 * @param sock
 * @return int 0 if success, 1 if error
 */
int receive_newpl(user* user_to_add, char* buff) {
  if (user_to_add == NULL) {
    return 1;
  }

  int sock = user_to_add->sock;
  int res = create_user(user_to_add, buff);
  if (res == 1) {
    return 1;
  }

  user_to_add->game_number = 0;
  game* new_game = create_game();
  new_game->list[0] = user_to_add;
  new_game->list[0]->game_number = count_game;

  // Ajouter la partie à la liste des parties
  pthread_mutex_lock(&mutex);
  if (games_list == NULL) {
    count_game = 1;
    games_list = malloc(sizeof(game*) * count_game);

    if (games_list == NULL) {
      die("ERROR : malloc games_list in receive_newpl");
    }
    // La première partie est à la 0ème position du tableau
    new_game->game_number = count_game - 1;
    games_list[count_game - 1] = new_game;
  } else {
    // On réalloue la taille de la liste
    game** new_list = realloc(games_list, sizeof(game*) * (count_game + 1));
    if (new_list == NULL) {
      die("ERROR : realloc games_list in receive_newpl");
    }
    new_game->game_number = count_game;
    new_list[count_game] = new_game;
    games_list = new_list;
    count_game++;
  }
  send_regok(sock, (uint8_t)(count_game - 1));
  pthread_mutex_unlock(&mutex);
  return 0;
}

/**
 * @brief Demande du joueur pour qu'il rejoigne une partie
 * [REGIS id port m***]
 * @param sock
 * @return int 0 if success 1 if not
 */
int receive_regis(user* user_to_add, char* buffer) {
  int offset = 20;
  uint8_t m;
  int res = create_user(user_to_add, buffer);
  if (res == 1) {
    send_regno(user_to_add->sock);
    return 1;
  }
  int sock = user_to_add->sock;
  memmove(&m, buffer + offset, 1);

  uint8_t numero = m;

  pthread_mutex_lock(&mutex);
  if (games_list == NULL) {
    send_regno(sock);
    pthread_mutex_unlock(&mutex);
    return 1;
  }
  if (games_list[numero] == NULL) {
    send_regno(sock);
    pthread_mutex_unlock(&mutex);
    return 1;
  }
  if (count_game == 0) {
    send_dunno(sock);
    pthread_mutex_unlock(&mutex);
    return 1;
  }
  if (numero >= count_game) {
    send_dunno(sock);
    pthread_mutex_unlock(&mutex);
    return 1;
  }

  if (games_list[numero]->statut == STATUS_ENCOURS ||
      games_list[numero]->statut == STATUS_FINI) {
    send_regno(sock);
    pthread_mutex_unlock(&mutex);
    return 1;
  }
  user_to_add->game_number = numero;

  user** new_list = realloc(
      games_list[numero]->list,
      sizeof(user*) * (games_list[numero]->connected_player_number + 1));
  if (new_list == NULL) {
    die("ERROR : realloc games_list in receive_regis");
  }
  games_list[numero]->list = new_list;
  games_list[numero]->list[games_list[numero]->connected_player_number] =
      user_to_add;
  games_list[numero]->connected_player_number++;
  pthread_mutex_unlock(&mutex);

  send_regok(sock, (uint8_t)numero);
  user_to_add->game_number = numero;
  return 0;
}

int receive_game(int sock) {
  //pthread_mutex_lock(&mutex);
  int games_in_waiting = getGamesInWaiting();
  //pthread_mutex_unlock(&mutex);

  if (games_in_waiting == 0) {
    send_games(sock, (uint8_t)0);
  } else {
    send_games(sock, (uint8_t)games_in_waiting);
  }
  return 0;
}

int receive_list(int sock, char* buff) {
  uint8_t m;
  memmove(&m, buff + 6, 1);
  // int game_number = (int)m - 48; // Convertir en int
  int game_number = (int)m;

  pthread_mutex_lock(&mutex);
  if (games_list == NULL) {
    send_dunno(sock);
    pthread_mutex_unlock(&mutex);
    return 1;
  }
  if (count_game == 0) {
    send_dunno(sock);
    pthread_mutex_unlock(&mutex);
    return 1;
  }
  if (game_number >= count_game) {
    send_dunno(sock);
    pthread_mutex_unlock(&mutex);
    return 1;
  }

  if (games_list[game_number] == NULL) {
    send_dunno(sock);
    pthread_mutex_unlock(&mutex);
    return 1;
  } else {
    int nbr_players = games_list[game_number]->connected_player_number;
    // send_list(sock, game_number, (uint8_t)nbr_players);
    send_list(sock, m, (uint8_t)nbr_players);
  }
  pthread_mutex_unlock(&mutex);
  return 0;
}

int receive_size(int sock, char* buff) {
  uint8_t m;
  memmove(&m, buff + 6, 1);
  // int game_number = (int)m - 48; // Convertir en int
  int game_number = (int)m;
  pthread_mutex_lock(&mutex);
  if (games_list == NULL) {
    send_dunno(sock);
    pthread_mutex_unlock(&mutex);
    return 1;
  }
  if (count_game == 0) {
    send_dunno(sock);
    pthread_mutex_unlock(&mutex);
    return 1;
  }
  if (game_number >= count_game) {
    send_dunno(sock);
    pthread_mutex_unlock(&mutex);
    return 1;
  }

  if (games_list[game_number] == NULL) {
    send_dunno(sock);
    pthread_mutex_unlock(&mutex);
    return 1;
  }

  game* game_to_send = games_list[game_number];
  // send_size(sock, game_number, game_to_send->lab->columns,
  // game_to_send->lab->lines);

  send_size(sock, m, game_to_send->lab->columns, game_to_send->lab->lines);
  pthread_mutex_unlock(&mutex);
  return 0;
}

command_client* receive_command(int sock) {
  command_client* command = malloc(sizeof(command_client));
  memset(command, 0, sizeof(command_client));
  if (command == NULL) {
    die("ERROR : malloc command in receive_command");
  }
  char *buf = malloc(100);
  if (buf == NULL) {
    die("ERROR : malloc buf in receive_command");
  }
  char opcode[6] = "";
  size_t buf_idx = 0;
  while (1) {
    if (buf_idx == 100) {
      printf("BUF IDX = BUF_SIZ Client disconnected\n");
      return NULL;
    }
    int rec = recv(sock, &buf[buf_idx], 1, 0);
    if (buf_idx > 1 && '*' == buf[buf_idx - 1] && '*' == buf[buf_idx - 2] &&
        '*' == buf[buf_idx]) {
      buf_idx++;
      break;
    }
    if (rec == 0) {
      return NULL;
    }
    buf_idx++;
  }
  memcpy(opcode, buf, 5);
  opcode[5] = '\0';

  if (strcmp(opcode, "NEWPL") == 0) {
    command->type = NEWPL;
  } else if (strcmp(opcode, "REGIS") == 0) {
    command->type = REGIS;
  } else if (strcmp(opcode, "GAME?") == 0) {
    command->type = GAMEH;
  } else if (strcmp(opcode, "LIST?") == 0) {
    command->type = LISTH;
  } else if (strcmp(opcode, "UNREG") == 0) {
    command->type = UNREG;
  } else if (strcmp(opcode, "SIZE?") == 0) {
    command->type = SIZE;
  } else if (strcmp(opcode, "START") == 0) {
    command->type = START;
  } else {
    command->type = -1;
  }
  command->size = buf_idx;
  command->data = malloc(sizeof(char) * (buf_idx));
  memset(command->data, 0, sizeof(char) * buf_idx);
  if (command->data == NULL) {
    die("ERROR : malloc command.data in receive_command");
  }
  memcpy(command->data, buf, buf_idx);
  memset(buf, 0, 100);
  free(buf);
  buf_idx = 0;
  return command;
}