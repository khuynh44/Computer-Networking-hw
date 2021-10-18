/**
 * Author:
 * GTID:
 * GT Email:
 */

#pragma once
typedef struct {
    int used;
    char username[100];
    struct sockaddr_in client;
} Client;

void *listClients(void *client);
int main(int argc, char *argv[]);
