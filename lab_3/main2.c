#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>
#define PATH "shmem_file"
#define ID 'Z'
#define SHMSIZE 1024

int main(int argc, char** argv) {
    key_t shmkey = ftok(PATH, ID);
    if (shmkey == -1) {
        perror("ftok");
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
    printf("PID: %d; TIME: %ld\nReceived string: %s\n", getpid(), time(NULL), shmdata);
    //shmctl(shmid, IPC_RMID, 0);
    return 0;
}
