/**
 * Author:
 * GTID:
 * GT Email:
 */
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include<iostream>
#include "chatclient.h"
#define MAX 256

struct sockaddr_in server;
int main(int argc, char *argv[]) {
  int socket_desc;
	
  int port = atoi(argv[5]);
	char username[20];
  char *hostname = argv[3];
  strcpy(username, argv[7]);
  char *passcode = argv[9];
	//Create socket
	if ((socket_desc = socket(AF_INET , SOCK_DGRAM , 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = inet_addr(hostname);
  int n;
  n = sendto(socket_desc, passcode, strlen(passcode), 
        MSG_CONFIRM, (struct sockaddr *) &server,
            sizeof(server));
  if (n < 0) {
    perror("first send failed");
    exit(EXIT_FAILURE);
  }
  char message[MAX];
  bzero(message, MAX);
  int len;
  //printf("waiting\n");
  n = recvfrom(socket_desc, message, MAX, 
                MSG_WAITALL, (struct sockaddr *) &server,
                (socklen_t* )&len);
  if (n < 0) {
    perror("first recieve failed");
    exit(EXIT_FAILURE);
  }
  if (strcmp(message, "Incorrect passcode") == 0) {
    puts(message);
    exit(EXIT_FAILURE);
  }
  sprintf(message, "%s joined the chatroom\n", username);
  printf("Connected to %s on port %d\n", hostname, port);
  if (sendto(socket_desc, (const char *)message, strlen(message), 
        MSG_CONFIRM, (const struct sockaddr *) &server, sizeof(server)) < 0) {
      perror("send failed");
      exit(EXIT_FAILURE);
  }
  
  pthread_t recv_id;
  pthread_create(&recv_id, NULL, &recv_thread, &socket_desc);
  while (1) {
    char con[500];
    bzero(con, 500);
    bzero(message, MAX);
    //sleep(1);
    fgets(message, MAX, stdin);
    if (message[0] == '\n') {
      continue;
    }
    time_t result = time(NULL);
    //char *time = asctime(gmtime(&result));
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
      bzero(message, MAX);
      struct tm* local = localtime(&result);
      local->tm_hour++;
      time_t time = mktime(local);
      strcpy(message, ctime(&time));
    }
    
    sprintf(con, "%s: %s", username, message);
    if (sendto(socket_desc, con, strlen(con), 
        MSG_CONFIRM, (const struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("second send failed");
        exit(EXIT_FAILURE);
    }
  }
  const char *exit = strcat(username, " left the chatroom\n");
  sendto(socket_desc, exit, strlen(exit), 
        MSG_CONFIRM, (const struct sockaddr *) &server, sizeof(server));
  pthread_cancel(recv_id);
  close(socket_desc);
  return 0;
}

void *recv_thread(void*socket_desc) {
  int socket = *(int*)socket_desc;
  char reply[MAX];
  //printf("out of while");
  while (1) {
     bzero(reply, MAX);
    //printf("made it");
    //fflush(stdout);
    socklen_t len;
    int n = recvfrom(socket, (char *)reply, MAX, 
                MSG_WAITALL, (struct sockaddr *) &server, &len);
      
    if (n < 0) {
      puts("server recieve error occured");
      break;
    }
    printf("%s", reply);
    //fflush(stdout);
	}
  pthread_cancel(pthread_self());
  return NULL;
}
