#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Call: %s <socket_file>\n", argv[0]);
    exit(1);
  }

  int create_socket;

  if ((create_socket = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
    goto socket_errors;
  }

  struct sockaddr_un unix_address;
  memset(&unix_address, 0, sizeof(unix_address));
  unix_address.sun_family = AF_LOCAL;

  printf("Connecting to socket at '%s'\n", argv[1]);

  int filename_len = strlen(argv[1]);
  if (filename_len + 1 > sizeof(unix_address.sun_path)) {
    errno = 0;
    goto socket_errors;
  }
  strncpy(unix_address.sun_path, argv[1], filename_len + 1);

  if (connect(create_socket, (struct sockaddr const *)&unix_address,
              sizeof(unix_address)) == -1) {
    goto socket_errors;
  }

  char signal;
  switch (read(create_socket, &signal, 1)) {
  case -1:
    goto socket_errors;
  case 0:
    printf("Server seems to be dead. Will unlink socket file and shut down.\n");
    unlink(unix_address.sun_path);
    return 0;
  default:
    break;
  }

  if (signal == 'x') {
    return 0;
  } else {
    printf("Received unexpected message: '%c'", signal);
    return 1;
  }

socket_errors:
  printf("[gnome-session-inhibitor] Error happened: %s\n", strerror(errno));
  exit(1);
}