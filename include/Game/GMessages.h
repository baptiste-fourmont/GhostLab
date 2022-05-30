#ifndef GAME_MESSAGE_H
#define GAME_MESSAGE_H

#include <Lobby/User.h>
#include <Server.h>
#include <Util.h>
#include <endian.h>

void send_game_message(int sockfd, char* buffer, int offset);
void send_gplyr(user* user, int sockfd);
void send_glis(user* user);
void send_mall(int sockfd);
void send_send(int sockfd);
void send_nsend(int sockfd);
int send_welcome(int sockfd, uint8_t m, uint16_t h, uint16_t w, uint8_t f,
                 char* ip, char* port);
void send_move_without_ghost(int sockfd, int x, int y);
void send_move_with_ghost(user* user, int x, int y);
void send_posit(user* user, int x, int y);
void send_gobye(int sockfd);

#endif
