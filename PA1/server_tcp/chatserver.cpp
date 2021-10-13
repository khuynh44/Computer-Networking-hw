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
  if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
  {
    puts("bind failed");
  }
  listen(socket_desc , 10);

  int c = sizeof(struct sockaddr_in);
  printf("Server started on port %d. Accepting connections\n", port);
  while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		printf("Server accepted");
		//Reply to the client
		char message[256];
		
		pthread_t sniffer_thread;
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
      char *error = "incorrect passcode";
      write(new_socket, error , strlen(error));
      close(new_socket);
      continue;
    } else {
      char *corr = "Correct";
      write(new_socket, corr , strlen(corr));
    }

    char username[256];
    if ((read_size = recv(new_socket , username, 255 , 0)) < 0) {
      puts("error with recieve");
      return 1;
    }
    Client *client;
    for (int i = 0; i < 10; i++) {
      if (clients[i].used == 0) {
        client = (clients + i);
        clients[i].used = 1;
        clients[i].username = username;
        clients[i].socket = new_socket;
        break;
      }
    }
    printf("%s joined the chatroom\n", username);
    pthread_t client_id;
    pthread_create(&client_id, NULL, &clientThread, client);
		// if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
		// {
		// 	perror("could not create thread");
		// 	return 1;
		// }
		
		//Now join the thread , so that we dont terminate before the thread
		//pthread_join( sniffer_thread , NULL);
		puts("Handler assigned");
	}
	
}
void *clientThread(void *client) {
  Client * client1 = (Client *)client;
  int read_size;
  char message[256];
  while ((read_size = recv(client1->socket , message , 256 , 0) > 0)) {
    for (int i = 0; i < 10; i++) {
      if (clients[i].used != 0 && clients[i].socket != client1->socket) {
        write(client1->socket , message , strlen(message));
      }
    }
    puts(message);
  }

}

