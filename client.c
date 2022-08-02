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
#define SHMSZ     4


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
void makeCommand(char* command, char* code, char* param1, char* param2);
void showMenuAction(char *deviceName, int MODE_DEFAULT, int MODE_SAVING);
void getShareMemoryPointer(char * key_from_server);
void runDevice(int defaultVoltage, int savingVoltage, char* deviceName,int isSaving);
void stopDevice(char *deviceName);
void switchMode(char *deviceName, char *mode, int newVoltage);
void getInfo(char * key_from_server);

int main(){
	getInfo("5678");
	strcpy(info, shm2);
	getInfo("9999");
	strcpy(systemInfo,shm2);
	char *token;
        token = strtok(systemInfo,"|");
	threshold = atoi(token);
	token = strtok(NULL,"|");
	maxThreshold = atoi(token);

	int shmid;
    key_t key;
    
    key = 2345;

    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((sumFirstVotage = shmat(shmid, NULL, 0)) == (int *) -1) {
        perror("shmat");
        exit(1);
    }
    

	struct sockaddr_in serverAddr;
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Kết nối bị lỗi.\n");
		exit(1);
	}
	printf("[+]Client Socket đã được kết nối.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0){
		printf("[-]Ket noi bi loi.\n");
		exit(1);
	}
	printf("[+]Đã kết nối thành công tới Server.\n");

	while(1){
		showMenuDevices();
	}

	return 0;
}

void showMenuDevices(){
        int choice;
        char c;
	char *a[3];
	char *token;
        while (1) {
                choice = 0;
                printf("\n-----------------ĐIÈU KHIỂN ĐIỆN NHÀ THÔNG MINH-------------------------\n");
                printf("Chọn thiết bị để sử dụng\n");
                printf("| 1. TV\n");
                printf("| 2. Điều hoà\n");
                printf("| 3. PC\n");
				printf("| 4. Thoát\n");
                printf("Mời bạn nhập mã tcủa thiết bị: ");
                while (choice == 0) {
                        if(scanf("%d",&choice) < 1) {
                                choice = 0;
                        }
                        if(choice < 1 || choice > 6) {
                                choice = 0;
                                printf("Thiết bị nhập không đúng!\n");
                                printf("Hãy nhập lại: ");
                        }
                        while((c = getchar())!='\n') ;
                }

                switch (choice) {
                case 1:
			token = strtok(info,",");
			a[0] = strtok(token,"|");
		        a[1] = strtok(NULL,"|");
		        a[2] = strtok(NULL,"|");
			showMenuAction(a[0],atoi(a[1]),atoi(a[2]));
			break;
                case 2:
			token = strtok(info,",");
			token = strtok(NULL,",");
			a[0] = strtok(token,"|");
			a[1] = strtok(NULL,"|");
			a[2] = strtok(NULL,"|");
			showMenuAction(a[0],atoi(a[1]),atoi(a[2]));
                        break;
		case 3:
			token = strtok(info,",");
			token = strtok(NULL,",");
			token = strtok(NULL,",");
			a[0] = strtok(token,"|");
			a[1] = strtok(NULL,"|");
			a[2] = strtok(NULL,"|");		
			showMenuAction(a[0],atoi(a[1]),atoi(a[2]));
			break;
                default:
                        exit(0);
                }
        }
}

void showMenuAction(char *deviceName, int MODE_DEFAULT, int MODE_SAVING) {
	int choice;
        char c;
        while (1) {
                choice = 0;
                printf("\n\n-----------------ĐIỀU KHIỂN NHÀ CHẾ ĐỘ THÔNG MINH-------------------------\n");
                printf("Chọn chế độ hoạt động của %s:\n", deviceName);
                printf("| 1. Chế độ bình thường - Công suất %d \n", MODE_DEFAULT);
                printf("| 2. Che do tiet kiem nang luong - Công suất %d (Chế độ này sẽ ảnh hưởng đến hiệu năng của thiết bị)\n", MODE_SAVING);
                printf("| 3. Tắt thiết bị và thoát\n");
                printf("Mời bạn chọn chế độ: ");
                while (choice == 0) {
                        if(scanf("%d",&choice) < 1) {
                                choice = 0;
                        }
                        if(choice < 1 || choice > 4) {
                                choice = 0;
                                printf("Chế độ không thích hợp!\n");
                                printf("Mời chọn lại: ");
                        }
                        while((c = getchar())!='\n') ;
                }
                switch (choice) {
		case 1:
			deviceName = strtok(deviceName,"|");
			firstVotage = MODE_DEFAULT;
			strcpy(firstDiviceName, deviceName);
			firstCase = 0;
			runDevice(MODE_DEFAULT,MODE_SAVING,deviceName,0);
			break;
                case 2:
			deviceName = strtok(deviceName,"|");
			firstVotage = MODE_SAVING;
			strcpy(firstDiviceName, deviceName);
			firstCase = 1;
			runDevice(MODE_DEFAULT,MODE_SAVING,deviceName,1);
			break;
                default:
                        exit(0);
                }
        }
}
void getResponse(){
      	int n = recv(clientSocket, serverResponse, MAXLINE, 0);
      	if (n == 0) {
	      perror("The server terminated prematurely");
	      exit(4);
      	}
      	serverResponse[n] = '\0';
}

int kbhit()
{
	struct timeval tv = { 0L, 0L };
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(0, &fds);
	return select(1, &fds, NULL, NULL, &tv);
}

int getch()
{
	    int r;
	    unsigned char c;
	    if ((r = read(0, &c, sizeof(c))) < 0) {
	        return r;
	    } else {
	        return c;
	    }
}

void makeCommand(char* command, char* code, char* param1, char* param2){
        strcpy(command, code);
        strcat(command, "|");
        if (param1 != NULL) {
                strcat(command,param1);
                strcat(command,"|");
        }
        if (param2 != NULL) {
                strcat(command,param2);
                strcat(command,"|");
        }
}

void getShareMemoryPointer(char * key_from_server){
	int shmid;
        key_t key;
	key = atoi(key_from_server);

	if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
	    perror("shmget");
	    exit(1);
	};
	if ((shm = shmat(shmid, NULL, 0)) == (int*) -1) {
		perror("shmat");
		exit(1);
	}
	lastShm = shm;
}

void getInfo(char * key_from_server){
	int shmid;
        key_t key;
	key = atoi(key_from_server);

	if ((shmid = shmget(key, 1000, 0666)) < 0) {
	    perror("shmget");
	    exit(1);
	}

	if ((shm2 = shmat(shmid, NULL, 0)) == (char*) -1) {
	    perror("shmat");
	    exit(1);
	}
}

void runDevice(int defaultVoltage, int savingVoltage, char *deviceName, int isSaving){
	char command[100];
	char response[100];
	char buffer[20];
	char param[20];
	int countDown;
	int currentVoltage;
	if(isSaving){
		strcat(deviceName,"|SAVING|");
		snprintf(buffer, 10, "%d", savingVoltage);
		currentVoltage = savingVoltage;
	} else {
		strcat(deviceName,"|NORMAL|");
		snprintf(buffer, 10, "%d", defaultVoltage);
		currentVoltage = defaultVoltage;
	}
	// snprintf(buffer, 10, "%d", voltage);
	makeCommand(command,"ON", deviceName,buffer);
	send(clientSocket, command, strlen(command), 0);
	getResponse();
	getShareMemoryPointer(serverResponse);
	countDown = 10;
	while (1) {
		if (*shm <= threshold){
			if(*sumFirstVotage <= threshold && check == 1){
				printf("Các thiết bị chuyển về chế độ ban đầu!\n");
				if(firstCase == 1){
					if (strstr(firstDiviceName, "|SAVING|") == NULL) {
					    strcat(firstDiviceName,"|SAVING|");
					}
				} else {
					if (strstr(firstDiviceName, "|NORMAL|") == NULL) {
					    strcat(firstDiviceName,"|NORMAL|");
					}
				}
				switchMode(firstDiviceName, "SWITCH",firstVotage);
				check = 0;
			}else{
				printf("Mức năng lượng tiêu thụ tại thời điểm hiện tại %d\n Ấn enter để ngắt thiết bị\n",*shm);
			}
			
		}
		else if(*shm <= maxThreshold){
			if(*sumFirstVotage <= maxThreshold && check == 1){
				printf("Các thiết bị chuyển về chế độ ban đầu!\n");
				if(firstCase == 1){
					if (strstr(firstDiviceName, "|SAVING|") == NULL) {
					    strcat(firstDiviceName,"|SAVING|");
					}
				} else {
					if (strstr(firstDiviceName, "|NORMAL|") == NULL) {
					    strcat(firstDiviceName,"|NORMAL|");
					}
				}
				switchMode(firstDiviceName, "SWITCH",firstVotage);
				check = 0;
			}else{
				printf("Đang vượt qua mức tiêu thụ ngưỡi cho phép. Mức tiêu thụ năng lượng hiện tại %d\n",*shm);
			}
		}
		else{
			if (!isSaving) {
				printf("Ngưỡng vượt qua, các thiết bị chuyển về chế độ tiết kiệm!\n");
				currentVoltage = savingVoltage;
				switchMode(deviceName, "SWITCH",currentVoltage);
				check = 1;
			}
			if (*shm > maxThreshold) {
				printf("Đã vượt qua mức tiêu thụ ngưỡng tối đã. Thiết bị sẽ bị dừng trong %d\n", countDown);
				countDown--;
				if(countDown < 0){
					stopDevice(deviceName);
					break;
				}
			}
		}

		if (kbhit()) {
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
	makeCommand(command,"STOP", deviceName, NULL);
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
