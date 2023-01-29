#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("Usage: %s STRING\n", argv[0]);
        return 0;
    }
    int fd[2];
    if (pipe(fd) == -1) {
        fprintf(stderr, "pipe error\n");
        return -1;
    }
    int cpid = fork();
    if (cpid > 0) {
        char str[1024];
        sprintf(str, "PID: %d\ntime: %ld\nstring: %s\n", getpid(), time(NULL), argv[1]);
        close(fd[0]);
        write(fd[1], str, strlen(str));
        wait(NULL);
    }
    else if (cpid == 0) {
        sleep(6);
        char msg[1025];
        close(fd[1]);
        int readNum = read(fd[0], msg, 1024);
        if (readNum == -1) {
            fprintf(stderr, "read error\n");
        }
        msg[readNum] = '\0';
        printf("[CHILD]: PID=%d, PPID=%d, time=%ld, received message:\n%s", getpid(), getppid(), time(NULL), msg);
    }
    else {
        fprintf(stderr, "fork error\n");
        return -1;
    }
}
