#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/sem.h>
#define PATH "shmem_file"
#define ID 'Z'
#define SHMSIZE 1024

// Destroy shared memory and semaphore
void sigintHandler(int s);

int main(int argc, char** argv) {
    signal(SIGINT, sigintHandler);
   // raise(SIGINT);
    //return 0;
    key_t shmkey = ftok(PATH, ID);
    if (shmkey == -1) {
        perror("ftok");
        return -1;
    }

    int semid = semget(shmkey, 1, 0644 | IPC_CREAT | IPC_EXCL);
    if (semid == -1) {
        perror("semget");
        return -1;
    }

    int shmid = shmget(shmkey, SHMSIZE, 0644 | IPC_CREAT | IPC_EXCL);
    if (shmid == -1) {
        perror("shmget");
        return -1;
    }
    char* shmdata = shmat(shmid, NULL, 0);
    if (shmdata == (void*) -1) {
        perror("shmat");
        return -1;
    }
    char buf[SHMSIZE];
    struct sembuf semBuf = {0, 0, 0};
    while (1) {
        // block writing to shm until second program read from shm
        if (semop(semid, &semBuf, 1) == -1) { // wait until second process sets semval = 0
            perror("semop");
            return -1;
        }
        printf("[%s]: wrote to shm\n", argv[0]);
        sprintf(buf, "PID: %d; TIME: %ld", getpid(), time(NULL));
        strcpy(shmdata, buf);
        sleep(3);
    }
    return 0;
}

void sigintHandler(int s) {
    key_t shmkey = ftok(PATH, ID);
    if (shmkey == -1) {
        perror("ftok");
        exit(-1);
    }
    int semid = semget(shmkey, 0, 0);
    if (semid == -1) {
        perror("semid");
        exit(-1);
    }
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl");
        exit(-1);
    }

    int shmid = shmget(shmkey, SHMSIZE, 0);
    if (shmid == -1) {
        perror("shmget");
        exit(-1);
    }
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl");
        exit(-1);
    }
    printf("===SIGINT: shared memory has been removed===\n");
    exit(1);
}
