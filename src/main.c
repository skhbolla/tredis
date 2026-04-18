#include "../include/client.h"
#include "../include/networking.h"
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <threads.h>
#include <unistd.h>

#define MAX_EVENTS 64

int main() {

  /**
   * Event loop implementation using epoll
   */

  // Disable output buffering
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  // Set up a non-blocking socket
  int sfd = setup_server_socket(6379);

  printf("socket setup with fd %d\n", sfd);

  // Initialize Epoll instance
  int epfd = epoll_create1(0);
  if (epfd < 0) {
    perror("epoll_create1: ");
    return -1;
  }

  // Register server socket with Epoll
  if (add_to_epoll(epfd, sfd) != 0) {
    perror("epoll_ctl: ");
    return -1;
  }

  struct epoll_event events[MAX_EVENTS];

  // Event loop to handle epoll events
  while (1) {
    int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
    for (int i = 0; i < n; i++) {
      if (events[i].data.fd == sfd) {
        // EPOLLIN event triggered on server fd
        // i.e new client
        int cfd = accept_and_register_client(epfd, sfd);
        if (cfd < 0) {
          int err = -cfd;

          if (err == EAGAIN || err == EWOULDBLOCK) {
            // This is expected for non-blocking
            continue;
          } else {
            fprintf(stderr, "New client failed to register: %s\n",
                    strerror(err));
            continue;
          }
        }

      } else {
        if (serve_client(events[i].data.fd) < 0) {
          close(events[i].data.fd); // closing fd auto removes from epoll
        }
      }
    }
  }
  return 0;
}
