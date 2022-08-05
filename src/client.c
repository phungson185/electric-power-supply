#include "../include/client.h"

int main()
{
  getInfo("5678");
  strcpy(info, shm2);
  getInfo("9999");
  strcpy(systemInfo, shm2);
  char *token;
  token = strtok(systemInfo, "|");
  threshold = atoi(token);
  token = strtok(NULL, "|");
  maxThreshold = atoi(token);

  int shmid;
  key_t key;

  key = 2345;

  if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0)
  {
    perror("shmget");
    exit(1);
  }

  if ((sumFirstVotage = shmat(shmid, NULL, 0)) == (int *)-1)
  {
    perror("shmat");
    exit(1);
  }

  struct sockaddr_in serverAddr;
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket < 0)
  {
    printf("[-]Connection Error.\n");
    exit(1);
  }
  printf("[+]Client Socket connected\n");

  memset(&serverAddr, '\0', sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
  {
    printf("[-]Connection Error.\n");
    exit(1);
  }
  printf("[+]Server connection successful.\n");

  while (1)
  {
    showMenuDevices();
  }

  return 0;
}

void showMenuDevices()
{
  int choice;
  char c;
  char *a[3];
  char *token;
  while (1)
  {
    choice = 0;
    printf("\n----------------Electric Power Supply-------------------------\n");
    printf("Device\n");
    printf("| 1. TV\n");
    printf("| 2. AC\n");
    printf("| 3. PC\n");
    printf("| 4. Radiator\n");
    printf("| 5. Dehumidifier\n");

    printf("| 6. Exit\n");
    printf("Choice: ");
    while (choice == 0)
    {
      if (scanf("%d", &choice) < 1)
      {
        choice = 0;
      }
      if (choice < 1 || choice > 6)
      {
        choice = 0;
        printf("Incorrect Device!\n");
        printf("Enter Again: ");
      }
      while ((c = getchar()) != '\n')
        ;
    }

    switch (choice)
    {
    case 1:
      token = strtok(info, ",");
      a[0] = strtok(token, "|");
      a[1] = strtok(NULL, "|");
      a[2] = strtok(NULL, "|");
      showMenuAction(a[0], atoi(a[1]), atoi(a[2]));
      break;
    case 2:
      token = strtok(info, ",");
      token = strtok(NULL, ",");
      a[0] = strtok(token, "|");
      a[1] = strtok(NULL, "|");
      a[2] = strtok(NULL, "|");
      showMenuAction(a[0], atoi(a[1]), atoi(a[2]));
      break;
    case 3:
      token = strtok(info, ",");
      token = strtok(NULL, ",");
      token = strtok(NULL, ",");
      a[0] = strtok(token, "|");
      a[1] = strtok(NULL, "|");
      a[2] = strtok(NULL, "|");
      showMenuAction(a[0], atoi(a[1]), atoi(a[2]));
      break;
    case 4:
      token = strtok(info, ",");
      token = strtok(NULL, ",");
      token = strtok(NULL, ",");
      token = strtok(NULL, ",");
      a[0] = strtok(token, "|");
      a[1] = strtok(NULL, "|");
      a[2] = strtok(NULL, "|");
      showMenuAction(a[0], atoi(a[1]), atoi(a[2]));
    case 5:
      token = strtok(info, ",");
      token = strtok(NULL, ",");
      token = strtok(NULL, ",");
      token = strtok(NULL, ",");
      token = strtok(NULL, ",");
      a[0] = strtok(token, "|");
      a[1] = strtok(NULL, "|");
      a[2] = strtok(NULL, "|");
      showMenuAction(a[0], atoi(a[1]), atoi(a[2]));
    default:
      exit(0);
    }
  }
}

void showMenuAction(char *deviceName, int MODE_DEFAULT, int MODE_SAVING)
{
  int choice;
  char c;
  while (1)
  {
    choice = 0;
    printf("\n\n-----------------Device Menu-------------------------\n");
    printf("%s's Mode:\n", deviceName);
    printf("| 1. Normal - Power %d \n", MODE_DEFAULT);
    printf("| 2. Saving - Power %d (This mode will affect to  device's performance)\n", MODE_SAVING);
    printf("| 3. Shutdown and exit\n");
    printf("Select mode: ");
    while (choice == 0)
    {
      if (scanf("%d", &choice) < 1)
      {
        choice = 0;
      }
      if (choice < 1 || choice > 3)
      {
        choice = 0;
        printf("Incorrect mode!\n");
        printf("Please choice again : ");
      }
      while ((c = getchar()) != '\n')
        ;
    }
    switch (choice)
    {
    case 1:
      deviceName = strtok(deviceName, "|");
      firstVotage = MODE_DEFAULT;
      strcpy(firstDiviceName, deviceName);
      firstCase = 0;
      runDevice(MODE_DEFAULT, MODE_SAVING, deviceName, 0);
      break;
    case 2:
      deviceName = strtok(deviceName, "|");
      firstVotage = MODE_SAVING;
      strcpy(firstDiviceName, deviceName);
      firstCase = 1;
      runDevice(MODE_DEFAULT, MODE_SAVING, deviceName, 1);
      break;
    default:
      exit(0);
    }
  }
}
void getResponse()
{
  int n = recv(clientSocket, serverResponse, MAXLINE, 0);
  if (n == 0)
  {
    perror("The server terminated prematurely");
    exit(4);
  }
  serverResponse[n] = '\0';
}

int kbhit()
{
  struct timeval tv = {0L, 0L};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(0, &fds);
  return select(1, &fds, NULL, NULL, &tv);
}

int getch()
{
  int r;
  unsigned char c;
  if ((r = read(0, &c, sizeof(c))) < 0)
  {
    return r;
  }
  else
  {
    return c;
  }
}

void makeCommand(char *command, char *code, char *param1, char *param2)
{
  strcpy(command, code);
  strcat(command, "|");
  if (param1 != NULL)
  {
    strcat(command, param1);
    strcat(command, "|");
  }
  if (param2 != NULL)
  {
    strcat(command, param2);
    strcat(command, "|");
  }
}

void getShareMemoryPointer(char *key_from_server)
{
  int shmid;
  key_t key;
  key = atoi(key_from_server);

  if ((shmid = shmget(key, SHMSZ, 0666)) < 0)
  {
    perror("shmget");
    exit(1);
  };
  if ((shm = shmat(shmid, NULL, 0)) == (int *)-1)
  {
    perror("shmat");
    exit(1);
  }
  lastShm = shm;
}

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

void runDevice(int defaultVoltage, int savingVoltage, char *deviceName, int isSaving)
{
  char command[100];
  char response[100];
  char buffer[20];
  char param[20];
  char tmpName[20];
  char firstName[20];
  int countDown;
  int currentVoltage;
  strcpy(tmpName, deviceName);
  if (isSaving)
  {
    strcat(deviceName, "|SAVING|");
    snprintf(buffer, 10, "%d", savingVoltage);
    currentVoltage = savingVoltage;
  }
  else
  {
    strcat(deviceName, "|NORMAL|");
    snprintf(buffer, 10, "%d", defaultVoltage);
    currentVoltage = defaultVoltage;
  }
  // snprintf(buffer, 10, "%d", voltage);
  makeCommand(command, "ON", deviceName, buffer);
  send(clientSocket, command, strlen(command), 0);
  getResponse();
  getShareMemoryPointer(serverResponse);
  countDown = 10;
  while (1)
  {
    strcpy(firstName, tmpName);
    if (*shm <= threshold)
    {
      if (*sumFirstVotage <= threshold && check == 1)
      {
        printf("Device return to original mode!\n");
        if (firstCase == 1)
        {
          if (strstr(firstDiviceName, "|SAVING|") == NULL)
          {
            strcat(firstDiviceName, "|SAVING|");
          }
        }
        else
        {
          if (strstr(firstDiviceName, "|NORMAL|") == NULL)
          {
            strcat(firstDiviceName, "|NORMAL|");
          }
        }
        switchMode(firstDiviceName, "SWITCH", firstVotage);
        check = 0;
      }
      else
      {
        printf("Current energy consumption %d\n Enter to shutdown\n", *shm);
      }
    }
    else if (*shm <= maxThreshold)
    {
      if (*sumFirstVotage <= maxThreshold && check == 1)
      {
        printf("Device return to original mode!\n");
        if (firstCase == 1)
        {
          if (strstr(firstDiviceName, "|SAVING|") == NULL)
          {
            strcat(firstDiviceName, "|SAVING|");
          }
        }
        else
        {
          if (strstr(firstDiviceName, "|NORMAL|") == NULL)
          {
            strcat(firstDiviceName, "|NORMAL|");
          }
        }
        switchMode(firstDiviceName, "SWITCH", firstVotage);
        check = 0;
      }
      else
      {
        printf("Exceeding allowable threshold. Current energy consumption %d\n", *shm);
      }
    }
    else
    {
      if (!isSaving)
      {
        printf("Threshold exceeded, all device turning to saving mode!\n");
        currentVoltage = savingVoltage;
        strcat(firstName, "|SAVING|"); /*bug day*/
        switchMode(firstName, "SWITCH", currentVoltage);
        check = 1;
      }
      if (*shm > maxThreshold)
      {
        printf("Supply exceeded. Device will be shutdown in %d\n", countDown);
        countDown--;
        if (countDown < 0)
        {
          stopDevice(deviceName);
          break;
        }
      }
    }

    if (kbhit())
    {
      stopDevice(deviceName);
      break;
    }
    sleep(1);
  }
}

void stopDevice(char *deviceName)
{
  *sumFirstVotage = *sumFirstVotage - firstVotage;
  char command[100];
  makeCommand(command, "STOP", deviceName, NULL);
  send(clientSocket, command, strlen(command), 0);
  getResponse();
}

void switchMode(char *deviceName, char *mode, int newVoltage)
{
  char command[100];
  char buffer[20];
  snprintf(buffer, 10, "%d", newVoltage);
  makeCommand(command, mode, deviceName, buffer);
  send(clientSocket, command, strlen(command), 0);
  getResponse();
}
