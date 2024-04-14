#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 3333
#define MAX_BUFFER_SIZE 1024
char *hello = "220 Welcome to Ganduth SMTP 0.1\r\n";

// handle_client
// Handles the client connection
void *handle_client(void *client_fd) {
  int new_socket = *(int *)client_fd;
  char buffer[MAX_BUFFER_SIZE] = {0};

  // Client Connected
  printf("Client Connected\n");

  // Send Welcome Message
  send(new_socket, hello, strlen(hello), 0);

  // Read Data
  int valread;
  while ((valread = read(new_socket, buffer, MAX_BUFFER_SIZE)) > 0) {
    printf("%s\n", buffer);

    if (strncmp(buffer, "EHLO", 4) == 0) {
      // Respond with a greeting and capabilities
      char *response =
          "250-localhost Hello localhost\r\n250 AUTH PLAIN LOGIN\r\n";
      send(new_socket, response, strlen(response), 0);
    }

    // Check if the command is QUIT
    if (strncmp(buffer, "QUIT", 4) == 0) {
      // Respond with a farewell message
      char *response = "221 Bye\r\n";
      send(new_socket, response, strlen(response), 0);
      break; // Exit the loop to close the connection
    }

    memset(buffer, 0, MAX_BUFFER_SIZE);
  }

  // Close the socket
  close(new_socket);
  free(client_fd);
  return NULL;
}

int main() {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[MAX_BUFFER_SIZE] = {0};
  // char *hello = "220 Welcome to Ganduth SMTP 0.1\r\n";

  // Create a socket
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Bind the socket to port
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  printf("%s, %d\n", "Waiting For New Connections", PORT);
  while (1) {

    // Accept incoming connections - Client Information
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int *client_fd = malloc(sizeof(int));

    // Accept Connections
    if ((*client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                             &client_addr_len)) < 0) {
      perror("accept failed");
      continue;
    }

    // Start A New Thread
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, handle_client, (void *)client_fd) <
        0) {
      perror("pthread_create failed");
      continue;
    }
    pthread_detach(thread_id);
  }

  // Close the socket
  close(new_socket);
  return 0;
}
