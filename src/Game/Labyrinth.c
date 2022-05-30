#include "Game/Labyrinth.h"
#include "Game/Multi.h"

#include <pthread.h>

labyrinth* create_labyrinth(uint16_t lines, uint16_t columns,
                            uint8_t nb_ghost) {
  labyrinth* laby = malloc(sizeof(labyrinth));
  laby->lines = lines;
  laby->columns = columns;
  laby->nb_ghost = nb_ghost;
  laby->tab = malloc(sizeof(node*) * lines);
  for (int i = 0; i < lines; i++) {
    laby->tab[i] = malloc(sizeof(node) * columns);
  }
  generate_maze(laby);
  return laby;
}

void initalize_labyrinthe(labyrinth* laby) {
  for (int i = 0; i < laby->lines; i++) {
    for (int j = 0; j < laby->columns; j++) {
      // Ligne colonne
      laby->tab[i][j].x = i;
      laby->tab[i][j].y = j;
      laby->tab[i][j].status = NONE;
    }
  }
}

void generate_maze(labyrinth* laby) {
  char maze[12][12] = {
      {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
      {'#', '.', '.', '.', '#', '.', '.', '.', '.', '.', '.', '#'},
      {'#', '.', '#', '.', '#', '.', '.', '#', '#', '#', '.', '#'},
      {'#', '#', '#', '.', '#', '.', '.', '.', '.', '.', '.', '#'},
      {'#', '.', '.', '.', '.', '#', '.', '#', '.', '#', '.', '#'},
      {'#', '#', '#', '#', '.', '#', '.', '#', '.', '#', '.', '#'},
      {'#', '.', '.', '#', '.', '#', '.', '#', '.', '#', '.', '#'},
      {'#', '#', '.', '#', '.', '#', '.', '#', '.', '#', '.', '#'},
      {'#', '.', '.', '.', '.', '.', '.', '.', '.', '#', '.', '#'},
      {'#', '#', '#', '#', '#', '#', '.', '#', '#', '#', '.', '#'},
      {'#', '.', '.', '.', '.', '.', '.', '#', '.', '.', '.', '#'},
      {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}};

  for (int i = 0; i < laby->lines; i++) {
    for (int j = 0; j < laby->columns; j++) {
      laby->tab[i][j].x = i;
      laby->tab[i][j].y = j;
      laby->tab[i][j].status = NONE;
    }
  }
  for (int i = 0; i < laby->lines; i++) {
    for (int j = 0; j < laby->columns; j++) {
      if (maze[i][j] == '#') {
        laby->tab[i][j].status = WALL;
      }
    }
  }

  for (int i = 0; i < laby->lines; i++) {
    for (int j = 0; j < laby->columns; j++) {
      if (maze[i][j] == '.') {
        laby->tab[i][j].status = NONE;
      }
    }
  }

  for (int i = 0; i < laby->nb_ghost; i++) {
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
      laby->tab[x][y].status = GHOST;
    } else {
      i--;
    }
  }
}

void print_labyrinth(labyrinth* laby) {
  printf("--0123456789xy\n");
  for (int i = 0; i < laby->lines; i++) {
    if (i < 10) {
      printf("0%d", i);
    } else {
      printf("%d", i);
    }
    for (int j = 0; j < laby->columns; j++) {
      if (laby->tab[i][j].status == WALL) {
        printf("#");
      } else if (laby->tab[i][j].status == GHOST) {
        printf("G");
      } else if (laby->tab[i][j].status == PLAYER) {
        printf("P");
      } else {
        printf(" ");
      }
    }
    printf("\n");
  }
}

labyrinth* move_ghost(labyrinth* laby, uint16_t i, uint16_t j) {
  // initialisation du tableau
  int** free_squares;
  free_squares = malloc(50 * sizeof(*free_squares));
  for (int k = 0; k < 50; k++) {
    free_squares[k] = malloc(2 * sizeof(free_squares[0]));
  }
  int k = 0;

  for (int x = 0; x < laby->lines; x++) {
    for (int y = 0; y < laby->columns; y++) {
      if (laby->tab[x][y].status == NONE &&
          (x == i || y == j)) { // Vérifier qu'il s'agit de la ligne ou de la
                                // colonne du fantome
        free_squares[k][0] = x;
        free_squares[k][1] = y;
        k++;
      }
    }
  }

  srand(time(NULL));
  int r = rand() % k;
  int x = free_squares[r][0];
  int y = free_squares[r][1];
  laby->tab[x][y].status = GHOST;
  laby->tab[i][j].status = NONE;
  char* send_msg = send_ghost(x, y);
  create_sender(laby->game_port, send_msg);
  return laby;
}

labyrinth* move_player(labyrinth* laby, user* user, uint16_t x, uint16_t y) {
  if (laby->tab[x][y].status == GHOST) {
    user->point += 100;
    laby->tab[x][y].status = NONE;
    laby->nb_ghost--;
  }

  laby->tab[x][y].status = PLAYER;
  laby->tab[x][y].user = user;
  laby->tab[user->x][user->y].status = NONE;
  laby->tab[user->x][user->y].user = NULL;
  user->x = x;
  user->y = y;

  int tab_ghost[laby->nb_ghost][2];
  int k = 0;

  for (int i = 0; i < laby->lines; i++) {
    for (int j = 0; j < laby->columns; j++) {
      if (laby->tab[i][j].status == GHOST) {
        tab_ghost[k][0] = i;
        tab_ghost[k][1] = j;
        k++;
      }
    }
  }

  for (int i = 0; i < laby->nb_ghost; i++) {
    move_ghost(laby, tab_ghost[i][0], tab_ghost[i][1]);
  }

  return laby;
}

node* get_node(labyrinth* laby, uint16_t x, uint16_t y) {
  return &laby->tab[x][y];
}

void delete_labyrinth(labyrinth* laby) {
  for (int i = 0; i < laby->lines; i++) {
    free(laby->tab[i]);
  }
  free(laby->tab);
  free(laby);
}