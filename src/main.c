#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void *handle_client(void *arg) {
  printf("Thread spawned!\n");
  int client_fd = *(int *)arg;
  free(arg);
  while (1) {
    // Read from the client
    char client_msg[1024];

    int x = recv(client_fd, client_msg, sizeof(client_msg), 0);
    if (x < 0) {
      perror("recv : ");
      break;
    } else if (x == 0) {
      // The client disconnected gracefully
      printf("Client disconnected gracefully\n");
      break;
    } else {
      // Send the response back
      char *msg = "+PONG\r\n";
      send(client_fd, msg, strlen(msg), 0);
    }
  }
  close(client_fd);
  return NULL;
}

int main() {
  // Disable output buffering
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.
  printf("Logs from your program will appear here!\n");

  int server_fd;
  socklen_t client_addr_len;
  struct sockaddr_in client_addr;

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    printf("Socket creation failed: %s...\n", strerror(errno));
    return 1;
  }

  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    printf("SO_REUSEADDR failed: %s \n", strerror(errno));
    return 1;
  }

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(6379),
      .sin_addr = {htonl(INADDR_ANY)},
  };

  if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
    printf("Bind failed: %s \n", strerror(errno));
    return 1;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    printf("Listen failed: %s \n", strerror(errno));
    return 1;
  }

  printf("Waiting for a client to connect...\n");
  client_addr_len = sizeof(client_addr);

  while (1) {
    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0) {
      perror("accept : ");
      continue;
    } else {
      char *client_ip = inet_ntoa(client_addr.sin_addr);
      int client_port = ntohs(client_addr.sin_port);
      printf("Client connected from %s : %d\n", client_ip, client_port);

      // Place a copy of client fd on heap
      int *client_heap_fd = malloc(sizeof(int));
      *client_heap_fd = client_fd;

      // Spawn a thread to serve a client
      pthread_t thread_id;
      if (pthread_create(&thread_id, NULL, handle_client,
                         (void *)client_heap_fd) != 0) {
        perror("Failed to create a thread!");
        free(client_heap_fd);
        continue;
      }

      // Detach so OS can auto clean up thread
      pthread_detach(thread_id);
    }
  }

  close(server_fd);

  return 0;
}
