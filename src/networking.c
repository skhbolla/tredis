#include "../include/networking.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

#define MAX_CONNECTION_BACKLOG 128

int setup_server_socket(int port) {
  // Create socket
  int server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  if (server_fd == -1) {
    perror("socket: ");
    return -1;
  }

  // set socket to reuse addr
  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  // Bind to port (assigning a name to server socket)
  struct sockaddr_in addr = {.sin_family = AF_INET,
                             .sin_port = htons(port),
                             .sin_addr = htonl(INADDR_ANY)};

  if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
    perror("bind: ");
    return -1;
  }

  // Set state to listen
  if (listen(server_fd, MAX_CONNECTION_BACKLOG) != 0) {
    perror("listen: ");
    return -1;
  }

  return server_fd;
}

int set_socket_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0); // 0 is basically ignored here
  int x = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  if (x == -1) {
    return -errno;
  } else {
    return x;
  }
}

int accept_new_client(int sfd) {
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  int cfd = accept(sfd, (struct sockaddr *)&client_addr, &client_addr_len);

  if (cfd == -1) {
    int err = -errno;
    return err;
  } else {
    char *client_ip = inet_ntoa(client_addr.sin_addr);
    int client_port = ntohs(client_addr.sin_port);
    printf("Client %s:%d connected\n", client_ip, client_port);
    return cfd;
  }
}
