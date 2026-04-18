#include "../include/client.h"
#include "../include/networking.h"
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
    // add NULL terminator
    buf[n] = '\0';
    printf("Received data from fd %d : %s", cfd, buf);

    // Return PONG response
    send(cfd, "+PONG\r\n", 7, MSG_DONTWAIT);
    return 0; // success

  } else if (n == 0) {
    // graceful disconnect (FIN) by client
    printf("Client %d gracefully disconnected\n", cfd);
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
