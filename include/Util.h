#ifndef ULTI_H
#define ULTI_H

#define LENGTH_PORT 4
#define LENGTH_IP 120
#define LENGTH_COORDONNE 24

#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void convert_to_pos(char* buffer, int value);
void convert_to_point(char* buffer, int value);
int convert_to_port(char* tab);
void die(const char* s);
char* complete_ip(char* str);
#endif
