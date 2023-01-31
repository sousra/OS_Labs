#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>
#include <sys/sem.h>
#define PATH "shmem_file"
#define ID 'Z'
#define SHMSIZE 1024

int main(int argc, char** argv) {
    key_t shmkey = ftok(PATH, ID);
    if (shmkey == -1) {
        perror("ftok");
        return -1;
    }
    
    int semid = semget(shmkey, 0, 0);
    if (semid == -1) {
        perror("semget");
        return -1;
    }

    int shmid = shmget(shmkey, SHMSIZE, 0);
    if (shmid == -1) {
        perror("shmget");
        return -1;
    }
    char* shmdata = shmat(shmid, NULL, 0);
    if (shmdata == (void*) -1) {
        perror("shmat");
        return -1;
    }

    struct sembuf semBuf = {0, 1, 0}; // {sem_num, sem_op, sem_flg}
    // block writing to shm in first programm 
    if (semop(semid, &semBuf, 1) == -1) {
        perror("semop");
        return -1;
    }
    printf("[%s]: blocked writing\n", argv[0]);
    sleep(10);
    printf("PID: %d; TIME: %ld\nReceived string: %s\n", getpid(), time(NULL), shmdata);
    semBuf.sem_op = -1;
    // unlock writing in first programm
    if (semop(semid, &semBuf, 1) == -1) {
        perror("semop");
        return -1;
    }
    printf("[%s]: unlocked writing\n", argv[0]);
    return 0;
}
