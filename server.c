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

void getInfo(char *key_from_server)
{
  int shmid;
  key_t key;
  key = atoi(key_from_server);

  if ((shmid = shmget(key, 1000, 0666)) < 0)
  {
    perror("shmget");
    exit(1);
  }

  if ((shm2 = shmat(shmid, NULL, 0)) == (char *)-1)
  {
    perror("shmat");
    exit(1);
  }
}

void sig_chld(int singno)
{
  pid_t pid;
  int stat;
  while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    printf("Client with port %d disconnected\n", ntohs(clientAddr.sin_port));
  return;
}

command convertRequestToCommand(char *request)
{
  char copy[100];
  printf("%s\n", request);
  strcpy(copy, request);
  command cmd;
  char *token;
  int i = 0;
  token = strtok(copy, "|");
  strcpy(cmd.code, token);
  while (token != NULL)
  {
    token = strtok(NULL, "|");
    if (token != NULL)
    {
      strcpy(cmd.params[i++], token);
    }
  }
  return cmd;
}

int main()
{
  int shmid;
  key_t key;
  int *shm;
  int *sumFirstVotage;
  key = atoi(KEY);
  int currentVoltage = 0;

  if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0)
  {
    perror("shmget");
    exit(1);
  }

  if ((shm = shmat(shmid, NULL, 0)) == (int *)-1)
  {
    perror("shmat");
    exit(1);
  }

  *shm = 0;

  int shmidVotage;
  key_t keyVotage;

  keyVotage = 2345;

  if ((shmidVotage = shmget(keyVotage, SHMSZ, IPC_CREAT | 0666)) < 0)
  {
    perror("shmget");
    exit(1);
  }

  if ((sumFirstVotage = shmat(shmidVotage, NULL, 0)) == (int *)-1)
  {
    perror("shmat");
    exit(1);
  }
  *sumFirstVotage = 0;

  getInfo("1111");
  if ((listenSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("Create socket error");
    exit(1);
  }
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(SERV_PORT);

  int enable = 1;
  if (setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    perror("setsockopt(SO_REUSEADDR) failed");

  if (bind(listenSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
  {
    printf("Bind socket error");
    exit(2);
  }

  listen(listenSock, LISTENQ);
  client = sizeof(clientAddr);

  while (1)
  {
    connectSock = accept(listenSock, (struct sockaddr *)&clientAddr, &client);
    if ((pid = fork()) == 0)
    {
      printf("Server connected with port %d\n", ntohs(clientAddr.sin_port));
      close(listenSock);
      int check = 0;
      while ((n = recv(connectSock, request, MAXLINE, 0)) > 0)
      {
        request[n] = '\0';
        cmd = convertRequestToCommand(request);
        if (strcmp(cmd.code, "STOP") == 0)
        {
          *shm = *shm - currentVoltage;
          currentVoltage = 0;
          sprintf(shm2, "%s|%s|%s|", cmd.params[0], "STOP", "0");
          check = 0;
        }
        else if (strcmp(cmd.code, "SWITCH") == 0)
        {
          sleep(1);
          sprintf(shm2, "%s|%s|%s|", cmd.params[0], cmd.params[1], cmd.params[2]);
          printf("%s\n", shm2);
          *shm = *shm - currentVoltage;
          currentVoltage = atoi(cmd.params[2]);
          *shm = *shm + currentVoltage;
        }
        else
        {
          sprintf(shm2, "%s|%s|%s|", cmd.params[0], cmd.params[1], cmd.params[2]);
          currentVoltage = atoi(cmd.params[2]);
          *shm = *shm + currentVoltage;
          if (check == 0)
          {
            *sumFirstVotage += currentVoltage;
            check = 1;
          }
        }
        send(connectSock, KEY, 4, 0);
      }
      if (currentVoltage != 0)
      {
        *shm = *shm - currentVoltage;
        currentVoltage = 0;
      }

      close(connectSock);
      exit(0);
    }
    signal(SIGCHLD, sig_chld);
    close(connectSock);
  }
}
