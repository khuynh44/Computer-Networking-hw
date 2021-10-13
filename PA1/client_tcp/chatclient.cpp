#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "chatclient.h"
/**
 * Author:
 * GTID:
 * GT Email:
 */
int main(int argc, char *argv[]) {

	int socket_desc;
	struct sockaddr_in server;
  int port = atoi(argv[5]);
	char *username = argv[7];
  char *passcode = argv[9];
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
    return 0;
	}
		
	server.sin_addr.s_addr = inet_addr("127.0.0.1");//address
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	//Connect to remote server
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
    close(socket_desc);
		return 1;
	}
	if(send(socket_desc , passcode , strlen(passcode) , 0) < 0)//send passcode
	{
		puts("Send failed");
    close(socket_desc);
		return 1;
	}
  char reply[200];
  bzero(reply, 199);
  if(recv(socket_desc, reply , 200 , 0) < 0)
	{
		puts("recv failed");
    close(socket_desc);
    return 1;
	}
  if (strcmp("incorrect passcode", reply) == 0) {
    puts(reply);
    close(socket_desc);
    return 1;
  }
  if(send(socket_desc , username , strlen(username) , 0) < 0)//send username
	{
		puts("Send failed");
    close(socket_desc);
		return 1;
	}
  pthread_t recv_id;
  pthread_create(&recv_id, NULL, &recv_thread, &socket_desc);
	printf("Connected to %s on port %d", argv[3], port);
  while(1) {
    char message[200];
    scanf("%s", message);
    if (strncmp(":Exit", message, 5) == 0) {
      break;
    }
    if (send(socket_desc, message , strlen(message) , 0) <= 0) {
      puts("server is down");
      break;
    }
  }
  pthread_cancel(recv_id);
  close(socket_desc);
	return 0;
}

void *recv_thread(void*socket_desc) {
  int socket = *(int*)socket_desc;
  char reply[200];
  printf("out of while");
  while (recv(socket, reply , 200 , 0) > 0)
	{
    printf("made it");
    printf("%s", reply);
	}
  return NULL;
}
