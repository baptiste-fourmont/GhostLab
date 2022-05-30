#ifndef LOBBY_MESSAGE_H
#define LOBBY_MESSAGE_H

#include <Server.h>
#include <Util.h>
#include <pthread.h>


void send_lobby_message(int sockfd, char* buffer, int offset);
void send_start(int sockfd);
void send_ogames(int sockfd, uint8_t m, uint8_t s);
void send_games(int sockfd, uint8_t n);
void send_regok(int sockfd, uint8_t m);
void send_regno(int sockfd);
void send_unrok(int sockfd, uint8_t m);
void send_dunno(int sockfd);
void send_size(int sockfd, uint8_t m, uint16_t w, uint16_t h);
void send_list(int sockfd, uint8_t m, uint8_t s);
void send_player(int sockfd, char* id);

#endif
