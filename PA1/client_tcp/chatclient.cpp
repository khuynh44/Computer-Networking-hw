#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include<iostream>
#include <time.h>
#include <stdint.h>
#include "chatclient.h"
#define MAX 256
/**
 * Author:
 * GTID:
 * GT Email:
 */
int main(int argc, char *argv[]) {

	int socket_desc;
	struct sockaddr_in server;
  int port = atoi(argv[5]);
	char username[MAX];
  char *hostname = argv[3];
  strcpy(username, argv[7]);
  char *passcode = argv[9];
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
    return 0;
	}
		
	server.sin_addr.s_addr = inet_addr(hostname);//address
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
  char reply[MAX];
  bzero(reply, MAX);
  if(recv(socket_desc, reply , 200 , 0) < 0)
	{
		puts("recv failed");
    close(socket_desc);
    return 1;
	}
  if (strcmp("Incorrect passcode", reply) == 0) {
    puts(reply);
    close(socket_desc);
    return 1;
  }
  //printf("%s %d", username, strlen(username));
  if(write(socket_desc , username , strlen(username)) < 0)//send username
	{
		puts("Send failed");
    close(socket_desc);
		return 1;
	}
  pthread_t recv_id;
  pthread_create(&recv_id, NULL, &recv_thread, &socket_desc);
	printf("Connected to %s on port %d\n", argv[3], port);
  while(1) {
    char message[MAX];
    bzero(message, MAX);
    //sleep(1);
    fgets(message, MAX, stdin);
    if (message[0] == '\n') {
      continue;
    }
    //trim(message, MAX);
    
    //scanf("%s", message);
    //cin >> message;
    time_t result = time(NULL);
    //char *time = ctime(&result);
    if (strcmp(":)\n", message) == 0) {
      strcpy(message, "[feeling happy]\n");
    } else if (strcmp(":(\n", message) == 0) {
      strcpy(message, "[feeling sad]\n");
    } else if (strcmp(":Exit\n", message) == 0) {
      break;
    } else if (strcmp(":mytime\n", message) == 0) {
      bzero(message, MAX);
      strcpy(message, ctime(&result));
      //sprintf(message, "%s", time);
    } else if (strcmp(":+1hr\n", message) == 0) {
      struct tm* local = localtime(&result);
      local->tm_hour++;
      time_t time = mktime(local);
      strcpy(message, ctime(&time));
    }
    if (send(socket_desc, message , strlen(message) , 0) <= 0) {
      puts("server is down");
      break;
    }
  }
  close(socket_desc);
	return 0;
  }


void *recv_thread(void*socket_desc) {
  int socket = *(int*)socket_desc;
  char reply[MAX];
  //printf("out of while");
  while (recv(socket, reply , MAX , 0) > 0)
	{
    //printf("made it");
    //fflush(stdout);
    printf("%s", reply);
    //fflush(stdout);
    bzero(reply, MAX);
	}
  pthread_cancel(pthread_self());
  return NULL;
}

void trim(char* arr, int length) {
  for (int i = 0; i < length; i++) { // trim \n
    if (arr[i] == '\n') {
      arr[i] = '\0';
      break;
    }
  }
}
