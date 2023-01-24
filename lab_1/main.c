#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

void sigintHandler(int status) {
    printf("[SIGINT was caught]: pid=%d, signal=%d (%s)\n", getpid(), status, strsignal(status));
}

void sigtermHandler(int status) {
    printf("[SIGTERM was caught]: pid=%d, signal=%d (%s)\n", getpid(), status, strsignal(status));
}

void exitFunction(void) {
    printf("[pid=%d]: process exit\n", getpid());
}

int main(int argc, char** argv) {
    atexit(exitFunction);
    signal(SIGINT, sigintHandler);
    struct sigaction action;
    action.sa_handler = sigtermHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGTERM, &action, NULL);

    pid_t cpid = fork();
    switch (cpid) {
        case (-1):
            printf("fork error\n");
            return -1;
            break;
        case (0):
            printf("[CHILD]: pid=%d, ppid=%d\n", getpid(), getppid());
            sleep(10);
            break;
        default:
            waitpid(cpid, NULL, 0);
            printf("[PARENT]: pid=%d, ppid=%d\n", getpid(), getppid());
            sleep(10);
    }
    return 0;
}
