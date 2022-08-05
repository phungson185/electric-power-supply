#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAXLINE 1024
#define PORT 3000
#define SHMSZ 4

int clientSocket;
char serverResponse[MAXLINE];
int *shm;
int *lastShm;
char *shm2;
// int currentVoltage;
char info[1000];
char systemInfo[1000];
int threshold;
int maxThreshold;
int firstVotage;
int firstCase;
char firstDiviceName[100];
int *sumFirstVotage;
int check = 0;

int kbhit();
int getch();
void showMenuDevices();
void showMenu();
void getResponse();
void makeCommand(char *command, char *code, char *param1, char *param2);
void showMenuAction(char *deviceName, int MODE_DEFAULT, int MODE_SAVING);
void getShareMemoryPointer(char *key_from_server);
void runDevice(int defaultVoltage, int savingVoltage, char *deviceName, int isSaving);
void stopDevice(char *deviceName);
void switchMode(char *deviceName, char *mode, int newVoltage);
void getInfo(char *key_from_server);

#endif