/**
 * Author:
 * GTID:
 * GT Email:
 */

#pragma once
typedef struct {
    int used;
    int socket;
    char *username;
} Client;

void *clientThread(void *client);
int main(int argc, char *argv[]);
