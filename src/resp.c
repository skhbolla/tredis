#include <stdio.h>

int parse_input(int n, char *msg, char *resp) {
  // add NULL terminator
  msg[n] = '\0';
  for (int i = 0; i < n; i++) {
    printf("[%02x] ", (unsigned char)msg[i]);
  }
  printf("\n");

  // parse the msg

  // 1. Check the first byte (data type)
  if (msg[0] == '*') {
    // Array data type

    int n = sprintf(resp, "+PONG\r\n");

    return n;
  }

  return -1;
}
