#include "Game/Tchat.h"

#define MAX_LEN 256          /* maximum receive string size */
#define MIN_PORT 1024        /* minimum port allowed */
#define MAX_PORT 65535       /* maximum port allowed */
#define GROUP "224.0.0.251"  // "225.0.0.1"  // 127.0.0.1

/**
 * @brief Create a sender object
 *
 * @param port
 * @param msg
 */
void create_sender(int port, char* msg) {
  int sock = socket(PF_INET, SOCK_DGRAM, 0);
  struct addrinfo* first_info;
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  char* port_ = malloc(sizeof(char) * 5);
  if (port_ == NULL) {
    die("Error port is null on create_sender");
  }
  port_[4] = '\0';
  sprintf(port_, "%d", port);

  int r = getaddrinfo(GROUP, port_, &hints, &first_info);
  if (r == 0) {
    if (first_info != NULL) {
      struct sockaddr* saddr = first_info->ai_addr;
      sendto(sock, msg, strlen(msg), 0, saddr,
             (socklen_t)sizeof(struct sockaddr_in));
    }
  }
  free(port_);
}