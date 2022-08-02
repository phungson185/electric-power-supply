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
#define SHMSZ    1024


char* readFileIntoString() {
        char * buffer = 0;
        long length;
        FILE * f = fopen ("systemInfo.txt", "rb");

        if (f)
        {
          fseek (f, 0, SEEK_END);
          length = ftell (f);
          fseek (f, 0, SEEK_SET);
          buffer = malloc (length);
          if (buffer)
          {
            fread (buffer, 1, length, f);
          }
          fclose (f);
        }

        if (buffer)
        {
          return buffer;
        }
}
main()
{
    char c;
    int shmid;
    key_t key;
    char *shm, *s;
    key = 9999;

    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
    s = shm;
    strcpy(s,readFileIntoString());
    exit(0);
}
