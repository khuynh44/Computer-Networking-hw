/**
 * Author:
 * GTID:
 * GT Email:
 */
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "chatserver.h"
#define MAX 256

Client clients[10];

int main(int argc, char *argv[]) {
  //bzero(clients, 10);
  int port = atoi(argv[3]);
  char message[MAX];
  char *passcode = argv[5];
  int socket_desc;
  struct sockaddr_in server, client;
  //Create socket
  bzero(message, MAX);
  socket_desc = socket(AF_INET , SOCK_DGRAM , 0);
  if (socket_desc == -1)
  {
    printf("Could not create socket");
    exit(EXIT_FAILURE);
  }
  memset(&server, 0, sizeof(server));
  memset(&client, 0, sizeof(client));
  memset(clients, 0, sizeof(Client)*10);
  //Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);

  if(bind(socket_desc, (struct sockaddr *)&server , sizeof(server)) < 0)
  {
    puts("bind failed");
    exit(EXIT_FAILURE);
  }

  int n;
  socklen_t len;
  len = sizeof(client);  //len is value/resuslt
  printf("Server started on port %d. Accepting connections\n", port);
  pthread_t list_id;
  pthread_create(&list_id, NULL, &listClients, NULL);
  while(1) {
    bzero(message, MAX);
    memset(&client, '\0', sizeof(client));
    //puts("Waiting to receive");
    n = recvfrom(socket_desc, message, MAX, 
                  0, (struct sockaddr *) &client,
                  &len);
    //fflush(stdout);
    //printf("%s",message);
    if (n < 0) {
      puts("recieve failed");
      exit(EXIT_FAILURE);
    }
    int flag = -1;
    int index = -1;
    for (int i = 0; i < 10; i++) {
      if (clients[i].used == 0) {
        index = i;
      } else if (clients[i].client.sin_port == client.sin_port) {
        //printf("%d        %d\n", client.sin_addr.s_addr, clients[i].client.sin_addr.s_addr);
        if (strlen(clients[i].username) == 0) {
          char temp[MAX];
          bzero(temp, MAX);
          strcpy(temp, message);
          for (size_t i = 0; i < strlen(temp); i++) {
            if (temp[i] == ' ') {
              temp[i] = '\0';
            }
          }
          strcpy(clients[i].username, temp);
        }
        flag = i;
      }
    }

    if (flag != -1) { //client is already in list
      for (int i = 0; i < 10; i++) {
        if (clients[i].used == 1 && i != flag) { //forward message to other clients
          sendto(socket_desc, message, strlen(message), 
              MSG_CONFIRM, (struct sockaddr *) &clients[i].client, sizeof(client));
        }
      }
      printf("%s", message);
      continue;
    }
    if (strcmp(message, passcode) != 0) {
      bzero(message, MAX);
      strcpy(message, "Incorrect passcode");
    } else {
      bzero(message, MAX);
      strcpy(message, "Correct passcode");
      clients[index].used = 1;
      clients[index].client = client;
    }
    sendto(socket_desc, message, strlen(message), 
            MSG_CONFIRM, (struct sockaddr *) &client, sizeof(client));
  }
  return 0;
}

void *listClients(void *client) {
  char buffer[256];
  while(1) {
    bzero(buffer, 256);
    fgets(buffer, 256, stdin);
    if (strcmp(buffer, "listclients\n") == 0) {
      for(int i = 0; i < 10; i++) {
        if (clients[i].used) {
          printf("%s ", clients[i].username);
        }
    }
    printf("\n");
  }
}
}