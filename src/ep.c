#include "../include/ep.h"
#include <errno.h>
#include <sys/epoll.h>

int add_to_epoll(int epfd, int fd) {
  struct epoll_event event;
  event.events = EPOLLIN;
  event.data.fd = fd;

  int x = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);

  if (x == -1) {
    return -errno;
  } else {
    return x;
  }
}
