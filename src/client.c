#include "../include/client.h"
#include "../include/networking.h"
#include "../include/resp.h"
#include <asm-generic/errno.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int accept_and_register_client(int epfd, int sfd) {
  int cfd = accept_new_client(sfd);
  if (cfd < 0)
    return cfd;

  int x = set_socket_nonblocking(cfd);
  if (x < 0) {
    close(cfd);
    return x;
  }

  x = add_to_epoll(epfd, cfd);
  if (x < 0) {
    close(cfd);
    return x;
  }

  return cfd;
}

int serve_client(int cfd) {
  char buf[4096];
  ssize_t n = recv(cfd, buf, sizeof(buf) - 1, MSG_DONTWAIT);

  if (n > 0) {

    char response_buffer[4068];
    int bytes_to_return = parse_input(n, buf, response_buffer);

    if (bytes_to_return > 0) {
      printf("%d bytes returned by parser as response\n", bytes_to_return);
      for (int i = 0; i < bytes_to_return; i++) {
        printf("[%02x] ", (unsigned char)response_buffer[i]);
      }
      printf("\n");
      // Return PONG response
      send(cfd, response_buffer, bytes_to_return, MSG_DONTWAIT);
      return 0; // success
    }

  } else if (n == 0) {
    // graceful disconnect (FIN) by client
    printf("Client %d gracefully disconnected (FIN)\n", cfd);
    return -1;

  } else {
    // error occured
    int err = errno;

    if (err == EAGAIN || err == EWOULDBLOCK) {
      // not actually an error..
      // just no more data to read
      return 0;
    } else {
      return -errno;
    }
  }
}
