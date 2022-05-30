#include "Util.h"
/**
 * @brief Permet de convertir en 3 octets
 * 11 => 011
 * char buffer[3];
 */
void convert_to_pos(char* buffer, int value) { sprintf(buffer, "%03d", value); }

/**
 * @brief Permet de convertir en 4 octets
 * 111 => 0111
 * char buffer[4];
 */
void convert_to_point(char* buffer, int value) {
  sprintf(buffer, "%04d", value);
}

/**
 * @brief Permet de convertir un tableau de char {"1",2","3","4"} en 1234
 *
 * @param tab
 * @return int // retourne le chiffre voulu sinon -1 si taille du tableau est
 * supérieur à 4
 */
int convert_to_port(char* tab) {
  if (strlen(tab) != 4) {
    return -1;
  }
  int i;
  sscanf(tab, "%d", &i);
  return i;
}

void die(const char* s) {
  perror(s);
  exit(1);
}

char* complete_ip(char* str) {
  if (str == NULL) {
    die("Error during malloc ip");
  }
  int len = strlen(str);
  char* res = malloc(15 - len + 1);
  if (res == NULL) {
    die("Error during completing ip");
  }

  for (int i = 0; i < 15 - len; i++) {
    strcat(res, "#");
  }
  strcat(str, res);
  str[15] = '\0';
  free(res);
  return str;
}