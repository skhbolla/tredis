#ifndef CLIENT_H
#define CLIENT_H

int accept_and_register_client(int epfd, int sfd);
int serve_client(int cfd);

#endif /* CLIENT_H */
