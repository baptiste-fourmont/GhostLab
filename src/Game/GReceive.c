#include <Game/GReceive.h>

command_game* receive_command_game(int sock) {
  command_game* command = malloc(sizeof(command_game));
  if (command == NULL) {
    die("ERROR : malloc command in receive_command");
  }
  char *buf = malloc(300);
  if (buf == NULL) {
    die("ERROR : malloc buf in receive_command");
  }
  char opcode[6] = "";

  size_t buf_idx = 0;
  while (1) {
    if (buf_idx == 300) {
      printf("BUF IDX = BUF_SIZ Client disconnected\n");
      return NULL;
    }
    int rec = recv(sock, &buf[buf_idx], 1, 0);
    if (buf_idx > 1 && '*' == buf[buf_idx - 1] && '*' == buf[buf_idx - 2] &&
        '*' == buf[buf_idx]) {
      printf("%c %c %c\n", buf[buf_idx], buf[buf_idx - 1], buf[buf_idx - 2]);
      break;
    }
    if (rec == 0) {
      return NULL;
    }
    buf_idx++;
  }
  memcpy(opcode, buf, 5);
  opcode[5] = '\0';

  if (strcmp(opcode, "UPMOV") == 0) {
    command->type = UPMOV;
  } else if (strcmp(opcode, "DOMOV") == 0) {
    command->type = DOMOV;
  } else if (strcmp(opcode, "LEMOV") == 0) {
    command->type = LEMOV;
  } else if (strcmp(opcode, "RIMOV") == 0) {
    command->type = RIMOV;
  } else if (strcmp(opcode, "IQUIT") == 0) {
    command->type = IQUIT;
  } else if (strcmp(opcode, "GLIS?") == 0) {
    command->type = GLIS;
  } else if (strcmp(opcode, "MALL?") == 0) {
    command->type = MALL;
  } else if (strcmp(opcode, "SEND?") == 0) {
    command->type = SEND;
  } else if (strcmp(opcode, "LEAD?") == 0) {
    command->type = LEAD;
  } else {
    command->type = -1;
  }
  command->size = buf_idx - 2;
  command->data = malloc(sizeof(char) * (buf_idx));
  if (command->data == NULL) {
    die("ERROR : malloc command.data in receive_command");
  }
  memcpy(command->data, buf, command->size);
  memset(buf, 0, 300);
  free(buf);
  buf_idx = 0;
  return command;
}

void advance_user(user* user, type_message_game type, int value) {
  int max;
  game* game = games_list[user->game_number];

  labyrinth* laby = game->lab;
  node* node;

  switch (type) {
  case UPMOV:
    if (user->x - value <= 0) {
      max = 1;
    } else {
      max = user->x - value;
    }
    break;
  case DOMOV:
    if (user->x + value >= game->lab->lines) {
      max = game->lab->lines - 1;
    } else {
      max = user->x + value;
    }
    break;
  case LEMOV:
    if (user->y - value <= 0) {
      max = 1;
    } else {
      max = user->y - value;
    }
    break;
  case RIMOV:
    if (user->y + value >= game->lab->columns) {
      max = game->lab->columns - 1;
    } else {
      max = user->y + value;
    }
    break;
  default:
    break;
  }

  int found = 0;

  switch (type) {
  case UPMOV:
    for (int i = user->x - 1; i >= max; i--) {
      node = get_node(laby, i, user->y);
      if (node->status == WALL || node->status == PLAYER) {
        break;
      } else {
        if (node->status == NONE || node->status == GHOST) {
          if (node->status == GHOST) {
            found = 1;
          }
          move_player(laby, user, i, user->y);
        }
      }
    }
    if (found == 1) {
      send_move_with_ghost(find_user(user->name), user->x, user->y);

      char* send_msg = send_score(user->name, find_user(user->name)->point,
                                  user->x, user->y);
      create_sender(atoi(game->port), send_msg);
      if (send_msg != NULL) {
        free(send_msg);
      }
    } else {
      send_move_without_ghost(user->sock, user->x, user->y);
    }
    break;
  case DOMOV:
    for (int i = user->x + 1; i <= max; i++) {
      node = get_node(laby, i, user->y);
      if (node->status == WALL || node->status == PLAYER) {
        break;
      } else {
        if (node->status == NONE || node->status == GHOST) {
          if (node->status == GHOST) {
            found = 1;
          }
          move_player(laby, user, i, user->y);
        }
      }
    }
    if (found == 1) {
      send_move_with_ghost(find_user(user->name), user->x, user->y);
      char* send_msg = send_score(user->name, find_user(user->name)->point,
                                  user->x, user->y);
      create_sender(atoi(game->port), send_msg);
      if (send_msg != NULL) {
        free(send_msg);
      }
    } else {
      send_move_without_ghost(user->sock, user->x, user->y);
    }
    break;
  case LEMOV:
    for (int i = user->y - 1; i >= max; i--) {
      node = get_node(laby, user->x, i);
      if (node->status == WALL || node->status == PLAYER) {
        break;
      } else {
        if (node->status == NONE || node->status == GHOST) {
          if (node->status == GHOST) {
            found = 1;
          }
          move_player(laby, user, user->x, i);
        }
      }
    }
    if (found == 1) {
      send_move_with_ghost(find_user(user->name), user->x, user->y);
      char* send_msg = send_score(user->name, find_user(user->name)->point,
                                  user->x, user->y);
      create_sender(atoi(game->port), send_msg);
      if (send_msg != NULL) {
        free(send_msg);
      }
    } else {
      send_move_without_ghost(user->sock, user->x, user->y);
    }
    break;
  case RIMOV:
    for (int i = user->y + 1; i <= max; i++) {
      node = get_node(laby, user->x, i);
      if (node->status == WALL || node->status == PLAYER) {
        break;
      } else {
        if (node->status == NONE || node->status == GHOST) {
          if (node->status == GHOST) {
            found = 1;
          }
          move_player(laby, user, user->x, i);
        }
      }
    }
    if (found == 1) {
      send_move_with_ghost(find_user(user->name), user->x, user->y);
      char* send_msg = send_score(user->name, find_user(user->name)->point,
                                  user->x, user->y);
      create_sender(atoi(game->port), send_msg);
      if (send_msg != NULL) {
        free(send_msg);
      }
    } else {
      send_move_without_ghost(user->sock, user->x, user->y);
    }
    break;
  default:
    break;
  }

  print_labyrinth(game->lab);
}

user* get_winner(game* game) {
  user* user = NULL;
  if (game->connected_player_number == 1) {
    return game->list[0];
  } else {
    for (int i = 0; i < game->connected_player_number; i++) {
      if (game->list[i]->point >= user->point) {
        user = game->list[i];
      }
    }
  }
  return user;
}

int receive_lead(user* user_) {
  FILE* fp = fopen("winner.txt", "r");
  if(fp==NULL){
    send_dunno(user_->sock);
    return -1;
  }
  struct user_save user_infile;
  while (fread(&user_infile, sizeof(struct user_save), 1, fp))
    printf("name = %s point = %d \n", user_infile.name, user_infile.point);
  fclose(fp);

  char* p_ = malloc(sizeof(char) * 5);
  p_[4] = '\0';
  convert_to_point(p_, user_infile.point);
  user_infile.name[8] = '\0';

  int offset = 0;
  char buffer[22];
  memmove(buffer, "LEAD!", 5);
  offset += 5;
  memmove(buffer + offset, " ", 1);
  offset += 1;
  memmove(buffer + offset, user_infile.name, 8);
  offset += 8;
  memmove(buffer + offset, " ", 1);
  offset += 1;
  memmove(buffer + offset, p_, 4);
  offset += 4;
  send_game_message(user_->sock, buffer, offset);
  if (p_ != NULL) {
    free(p_);
  }
  return 0;
}

void checkGhost(int game_number, user* user_) {
  printf("Check Ghost for user: %s\n", user_->name);

  game* game = games_list[game_number];

  if (game->lab->nb_ghost == 0) {
    game->statut = STATUS_FINI;
    user* winner = get_winner(game);

    FILE* fp;
    struct user_save user_infile;

    fp = fopen("winner.txt", "r");
    if (fp != NULL) {
      while (fread(&user_infile, sizeof(struct user), 1, fp) != 0)
        ;
      fclose(fp);
      printf("%d\n", user_infile.point);
      if (user_infile.point < winner->point) {
        fp = fopen("winner.txt", "w");
        if (fp == NULL) {
          die("Error during fopen");
        }
        strcpy(user_infile.name, winner->name);
        user_infile.name[8] = '\0';
        user_infile.point = winner->point;
        fwrite(&user_infile, sizeof(struct user_save), 1, fp);
        fclose(fp);
      }
    } else {
      fp = fopen("winner.txt", "w");
      if (fp == NULL) {
        die("Error during fopen");
      }
      user_infile.point = winner->point;
      strcpy(user_infile.name, winner->name);
      user_infile.name[8] = '\0';
      printf("%d\n", user_infile.point);
      printf("%s\n", user_infile.name);
      fwrite(&user_infile, sizeof(struct user_save), 1, fp);
      fclose(fp);
    }

    char* msg = send_enga(winner->name, winner->point);
    create_sender(atoi(game->port), msg);
    if (msg != NULL) {
      free(msg);
    }
    // On décale la game
    for (int i = 0; i < game->connected_player_number; i++) {
      send_gobye(game->list[i]->sock);
      close(game->list[i]->sock);
    }

    // On envoie le message de fin de partie
    game->statut = STATUS_FINI;
    free(game->list);
    free(game->port);
    free(game->ip);
    free(game->lab);
    free(games_list[game_number]);

    for (int i = game_number; i < count_game; i++) {
      games_list[i] = games_list[i + 1];
    }
    count_game--;
    return;
  }
}

int receive_upmov(user* user, char* buff) {
  char buffer[3];
  buffer[3] = '\0';
  memmove(buffer, buff + 6, 3);

  int y = atoi(buffer);
  
  pthread_mutex_lock(&mutex);
  advance_user(user, UPMOV, y);
  pthread_mutex_unlock(&mutex);
  return 0;
}

int receive_domov(user* user, char* buff) {
  char buffer[3];
  buffer[3] = '\0';
  memmove(buffer, buff + 6, 3);

  int y = atoi(buffer);
  pthread_mutex_lock(&mutex);
  advance_user(user, DOMOV, y);
  pthread_mutex_unlock(&mutex);
  return 0;
}
int receive_lemov(user* user, char* buff) {
  char buffer[3];
  buffer[3] = '\0';
  memmove(buffer, buff + 6, 3);

  int x = atoi(buffer);
  pthread_mutex_lock(&mutex);
  advance_user(user, LEMOV, x);
  pthread_mutex_unlock(&mutex);
  return 0;
}

int receive_rimov(user* user, char* buff) {
  char buffer[3];
  buffer[3] = '\0';
  memmove(buffer, buff + 6, 3);

  int x = atoi(buffer);
  pthread_mutex_lock(&mutex);
  advance_user(user, RIMOV, x);
  pthread_mutex_unlock(&mutex);

  return 0;
}

int receive_glis(user* user) {
  send_glis(user);
  return 0;
}

int receive_mall(user* user, char* buff) {
  game* game = games_list[user->game_number];
  int offset = 6;
  char* buffer = malloc(sizeof(char) * strlen(buff)+1);
  if (buffer == NULL) {
    die("malloc in receive_mall");
  }
  buffer[strlen(buff)] = '\0';
  memmove(buffer, buff + offset, strlen(buff));
  send_mall(user->sock);
  char* send_msg = send_messa(user->name, buffer);
  create_sender(atoi(game->port), send_msg);
  if (buffer != NULL) {
    free(buffer);
  }
  if (send_msg != NULL) {
    free(send_msg);
  }
  return 0;
}

int receive_send(user* user, char* buff) {
  int offset = 6;
  char* buffer = malloc(sizeof(char) * strlen(buff)+1);
  if (buffer == NULL) {
    die("malloc in receive_send");
  }
  buffer[strlen(buff)] = '\0';

  char* id = malloc(sizeof(char) * 9);
  if (id == NULL) {
    die("malloc in receive_send");
  }

  id[8] = '\0';
  memmove(id, buff + offset, 8);
  offset += 9;
  memmove(buffer, buff + offset, strlen(buff));
  send_messp(user, id, buffer);
  if (buffer != NULL) {
    free(buffer);
  }
  if (id != NULL) {
    free(id);
  }
  return 0;
}