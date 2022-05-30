#ifndef LABYRINTH_H
#define LABYRINTH_H

#include <Lobby/User.h>
#include <Util.h>

typedef enum status_node { NONE, WALL, GHOST, PLAYER } status_node;

typedef struct ghost {
  int can_move;
} ghost;

typedef struct node {
  uint16_t x;
  uint16_t y;
  user* user;
  ghost* ghost;
  status_node status;
} node;

typedef struct labyrinth {
  uint16_t lines;
  uint16_t columns;
  uint8_t nb_ghost;
  node** tab;
  int game_port;
} labyrinth;

labyrinth* create_labyrinth(uint16_t lines, uint16_t columns, uint8_t nb_ghost);
void initalize_labyrinthe(labyrinth* laby);
void generate_maze(labyrinth* laby);
void print_labyrinth(labyrinth* laby);
labyrinth* move_ghost(labyrinth* laby, uint16_t i, uint16_t j);
labyrinth* move_player(labyrinth* laby, user* user, uint16_t x, uint16_t y);
node* get_node(labyrinth* laby, uint16_t x, uint16_t y);
void delete_labyrinth(labyrinth* laby);

#endif