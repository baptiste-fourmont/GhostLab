#ifndef MULTI_H
#define MULTI_H

#include <Server.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>

void send_udp(const char* msg, int port);
void send_multi(int sockfd, char* buffer, int offset, struct sockaddr* saddr);
int send_messp(user* ask, char* id, char* msg);
char* send_messa(char* id, char* msg);
char* send_ghost(int x, int y);
char* send_enga(char* id, int p);
char *send_score(char* id, int p, int x, int y);

#endif