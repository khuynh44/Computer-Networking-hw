/**
 * Author:
 * GTID:
 * GT Email:
 */

#pragma once
typedef struct {
    int used;
    int socket;
    char username[256];
} Client;

void *clientThread(void *client);
void *listClients(void *client);
int main(int argc, char *argv[]);
