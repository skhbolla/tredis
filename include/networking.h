#ifndef NETWORKING_H
#define NETWORKING_H

int setup_server_socket(int port);
int add_to_epoll(int epfd, int fd);
int accept_new_client(int sfd);
int set_socket_nonblocking(int fd);

#endif /* NETWORKING.H */
