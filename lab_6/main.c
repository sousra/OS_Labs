#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#define READTHREAD_SIZE 10

char sigint = 0;

void sigintHandler(int s) {
    sigint = 1;
}

typedef struct funcArg {
    char data[32];
    int counter; 
} funcArg;
void* writeFunc(void* arg);
void* readFunc(void* arg);

pthread_mutex_t mutex;
pthread_cond_t cond;

int main(int argc, char** argv) {
    srand(time(NULL));
    signal(SIGINT, sigintHandler);

    funcArg arg = {"0", 0};
    pthread_t writeThread;
    pthread_t readThread[READTHREAD_SIZE];
    
    if (pthread_mutex_init(&mutex, NULL)) { fprintf(stderr, "pthread_mutex_init error\n"); return -1; }
    if (pthread_cond_init(&cond, NULL)) { fprintf(stderr, "pthread_cond_init error\n"); return -1; }

    while (!sigint) {
        // Create threads
        printf("[CYCLE BEGIN]\n");
        
        if (pthread_create(&writeThread, NULL, writeFunc, (void*) &arg)) {
                fprintf(stderr, "pthread_create error\n");
                return -1;
        }
        
        for (int i = 0; i < READTHREAD_SIZE; ++i) {
            if (pthread_create(&readThread[i], NULL, readFunc, (void*) arg.data)) {
                fprintf(stderr, "pthread_create error\n");
                return -1;
            }
        }

        // Join threads
        if (pthread_join(writeThread, NULL)) {
                fprintf(stderr, "pthread_join error\n");
                return -1;
        }
        for (int i = 0; i < READTHREAD_SIZE; ++i) {
            if (pthread_join(readThread[i], NULL)) {
                fprintf(stderr, "pthread_join error\n");
                return -1;
            }
        }
        printf("[CYCLE END]: counter value = %s\n", arg.data);
        sleep(3);
    }

    if (pthread_mutex_destroy(&mutex)) { fprintf(stderr, "pthread_mutex_destroy error\n"); return -1; }
    if (pthread_cond_destroy(&cond)) { fprintf(stderr, "pthread_cond_destroy error\n"); return -1; }
    return 0;
}

void* writeFunc(void* arg) {
    if (pthread_mutex_lock(&mutex)) { fprintf(stderr, "pthread_mutex_lock error\n");  exit(-1); }
    printf("before condwait\n");
    if (pthread_cond_wait(&cond, &mutex)) { fprintf(stderr, "pthread_cond_wait error\n");  exit(-1); }
    printf("after condwait\n");
    funcArg* a = (funcArg*) arg;
    ++(a->counter);
    sprintf(a->data, "Note number %d", a->counter);
    sleep(2); // Без мютексов читающие потоки бы использовали значение, полученное в этой функции, до того как она завершилась - не хорошо
    printf("Write thread: %lu\n", pthread_self());    
    if (pthread_mutex_unlock(&mutex)) { fprintf(stderr, "pthread_mutex_unlock error\n");  exit(-1); }
}

void* readFunc(void* arg) {
    if (pthread_mutex_lock(&mutex)) { fprintf(stderr, "pthread_mutex_lock error\n");  exit(-1); }
    sleep(1);
    printf("[%lu]: %s\n", pthread_self(), (char*) arg);
    if (pthread_cond_signal(&cond)) { fprintf(stderr, "pthread_cond_signal error\n");  exit(-1); }
    if (pthread_mutex_unlock(&mutex)) { fprintf(stderr, "pthread_mutex_unlock error\n"); exit(-1); }
}
