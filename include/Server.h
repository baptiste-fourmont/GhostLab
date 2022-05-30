#ifndef SERVER_H
#define SERVER_H

#define MAX_GAMES 10

#include <Game/GMessages.h>
#include <Game/GReceive.h>
#include <Game/Labyrinth.h>
#include <Game/Multi.h>
#include <Game/Tchat.h>
#include <Lobby/Game.h>
#include <Lobby/LMessages.h>
#include <Lobby/LReceive.h>
#include <Lobby/User.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

void* handle_client(void* args);
int interact_lobby(user* user, int mode);
int interact_game(user* user, int mode);

extern pthread_mutex_t mutex;

#endif