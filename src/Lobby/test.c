#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void printMaze();
typedef struct way way;
struct way {
  int x;
  int y;
  char c;
  int marque;
  way* next;
};

int width = 0;
int height = 0;
way* maze = NULL;

static way* BuildWay(int x, int y, char c, int m) {
  way* w = malloc(sizeof(way));
  assert(w != NULL);
  w->x = x;
  w->y = y;
  w->c = c;
  w->marque = 0;
  return w;
}

static void setWay(way* w, char c) {
  assert(w != NULL);
  w->c = c;
}

static way* getWay(way* maze, int x, int y) {
  way* result = NULL;
  way* tmp = maze;
  while (tmp != NULL) {
    if ((tmp->x == x) && (tmp->y == y)) {
      result = tmp;
    }
    tmp = tmp->next;
  }
  return result;
}

static way* add2Maze(way* m, way* w) {
  if (m == NULL) {
    m = w;
  } else {
    m->next = add2Maze(m->next, w);
  }
  return m;
}
static void fillMaze() {
  srand(time(NULL));
  int w = rand() % (20 - 10) + 10;
  int h = rand() % (20 - 10) + 10;
  width = w;
  height = h;
  // char* result = malloc(w * h * sizeof(char));
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      way* w = BuildWay(i, j, '#', 0);
      maze = add2Maze(maze, w);
      //*(result + (i * w) + j) = '#';
    }
  }
}

static char randomMove(way* w) {
  int randIndex;
  char* tab = malloc(sizeof(char));
  if (w->x == 0 && w->y == 0) {
    strcpy(tab, "rd");
  } else if (w->x == height - 1 && w->y == 0) {
    strcpy(tab, "ur");
  } else if (w->x == 0 && w->y == width - 1) {
    strcpy(tab, "ld");
  } else if (w->x == height - 1 && w->y == width - 1) {
    strcpy(tab, "lu");
  } else if (w->x == 0 && w->y != 0 && w->y != width - 1) {
    strcpy(tab, "ldr");
  } else if (w->x != height - 1 && w->x != 0 && w->y == 0) {
    strcpy(tab, "udr");
  } else if (w->y != 0 && w->y == width - 1) {
    strcpy(tab, "udl");
  } else if (w->x == height - 1 && w->y != width - 1) {
    strcpy(tab, "ulr");
  } else {
    strcpy(tab, "udlr");
  }
  randIndex = rand() % strlen(tab);
  return *(tab + randIndex);
}

static void addWay() {
  srand(time(NULL));
  int num = 0;
  int max = (width * height) / 2;
  int min = ((width * height) / 2) - 5;
  int randWay = rand() % (max - min) + min;
  int x = rand() % height;
  int y = rand() % width;
  setWay(getWay(maze, x, y), 'G');
  getWay(maze, x, y)->marque = 1;
  printf("ok\n");
  while (num < randWay) {
    printMaze(maze);
    printf("valX = %d et valY  = %d\n", x, y);
    printf("ok\n");
    if (randomMove(getWay(maze, x, y)) == 'l') {
      printf("ll\n");
      x = getWay(maze, x, y)->x;
      y = getWay(maze, x, y)->y - 1;
      if (getWay(maze, x, y)->marque != 1) {
        setWay(getWay(maze, x, y), 'G');
      } else {
      }
    } else if (randomMove(getWay(maze, x, y)) == 'r') {
      printf("rr\n");
      x = getWay(maze, x, y)->x;
      y = getWay(maze, x, y)->y + 1;
      setWay(getWay(maze, x, y), 'G');
    } else if (randomMove(getWay(maze, x, y)) == 'u') {
      printf("uu\n");
      x = getWay(maze, x, y)->x + 1;
      y = getWay(maze, x, y)->y;
      setWay(getWay(maze, x, y), 'G');
    } else if (randomMove(getWay(maze, x, y)) == 'd') {
      printf("dd\n");
      x = getWay(maze, x, y)->x - 1;
      y = getWay(maze, x, y)->y;
      setWay(getWay(maze, x, y), 'G');
    }
    num++;
  }
}

static void printMaze(way* maze) {
  way* tmp = maze;
  // int count = 0;
  while (tmp != NULL) {
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        printf("%c", tmp->c);
        tmp = tmp->next;
      }
      printf("\n");
    }
  }
}

int main(void) {
  fillMaze();
  printf("1er tab\n");
  // printMaze(maze);
  //  printf("%c\n", randomMove(getWay(maze, height - 1, width - 1)));
  addWay();
  /*printf("2eme tab\n");
  printMaze(maze);*/
}