#ifndef _SERVER_H_
#define _SERVER_H_

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSZ 4
#define KEY "1234"
#define MAXLINE 1024   /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define LISTENQ 8      /*maximum number of client connections*/

typedef struct
{
    char code[100];
    char params[3][256];
} command;

int listenSock, connectSock, n;
pid_t pid;
char request[MAXLINE];
struct sockaddr_in serverAddr, clientAddr;
socklen_t client;
command cmd;
char *shm2;

#endif