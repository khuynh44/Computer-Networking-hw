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


Client *clients;


int main(int argc, char *argv[]) {
  int port = atoi(argv[3]);
  char *passcode = argv[5];
  int socket_desc, new_socket;
  struct sockaddr_in server, client;
  clients = (Client *)malloc(sizeof(Client)*10);
  bzero(clients, sizeof(Client)*10);
  //Create socket
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1)
  {
    printf("Could not create socket");
  }
  
  //Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
    
  //Bind
  if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
  {
    puts("bind failed");
    close(socket_desc);
    return 0;
  }
  listen(socket_desc , 10);

  int c = sizeof(struct sockaddr_in);
  printf("Server started on port %d. Accepting connections\n", port);
  pthread_t list_id;
  pthread_create(&list_id, NULL, &listClients, NULL);
  while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		//printf("Server accepted");
		//Reply to the client
		char message[256];

    if (new_socket < 0) {
		  puts("accept failed");
		  return 1;
	  }
    int read_size;
		if ((read_size = recv(new_socket , message , 256 , 0)) < 0) {
      puts("error with recieve");
      return 1;
    }
    if (strcmp(passcode, message) != 0) {
      write(new_socket, "Incorrect passcode", 18);
      close(new_socket);
      continue;
    } else {
      write(new_socket, "Correct", 7);
    }

    char username[256];
    bzero(username, 256);
    if ((read_size = recv(new_socket , username, 256 , 0)) < 0) {
      puts("error with recieve");
      return 1;
    }
    Client *clien;
    int flag = 0;
    printf("%s joined the chatroom\n", username);
    for (int i = 0; i < 10; i++) {
      if (clients[i].used == 0 && flag == 0) {
        flag = 1;
        clien = (clients + i);
        clients[i].used = 1;
        strcpy(clients[i].username, username);
        clients[i].socket = new_socket;
      } else if (clients[i].used == 1) {
        char *join = strcat(username," joined the chatroom\n");
        write(clients[i].socket , join, strlen(join));
      }
    }
    //printf("%s joined the chatroom\n", username);
    pthread_t client_id;
    pthread_create(&client_id, NULL, &clientThread, (void*)clien);
		
		//Now join the thread , so that we dont terminate before the thread
		//pthread_join( sniffer_thread , NULL);
		//puts("Handler assigned");
	}
  close(socket_desc);
	
}
void *clientThread(void *client) {
  Client *client1 = (Client *)client;
  int read_size;
  char message[256];
  while ((read_size = recv(client1->socket , message , 256 , 0) > 0)) {
    char p[520];
    bzero(p, 520);
    sprintf(p, "%s: %s", client1->username, message);
    for (int i = 0; i < 10; i++) {
      if (clients[i].used != 0 && (clients + i) != client1) {
        write(clients[i].socket , p, strlen(p));
      }
    }
    printf("%s", p);
    bzero(message, 256);
    //fflush(stdout);
  }
  client1->used = 0;
  char p[300];
  bzero(p, 300); 
  sprintf(p, "%s left the chatroom\n", client1->username);
  printf("%s", p);
  for (int i = 0; i < 10; i++) {
    if (clients[i].used != 0) {
      write(clients[i].socket , p, strlen(p));
    }
  }
  close(client1->socket);
  pthread_cancel(pthread_self());
  return NULL;
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
