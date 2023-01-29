#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFOFILE "temp_file"

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("Usage: %s STRING\n", argv[0]);
        return 0;
    }

    if (mkfifo(FIFOFILE, 0777) == -1) {
        fprintf(stderr, "fifo error\n");
        return -1;
    }

    int fd;
    int cpid = fork();
    if (cpid > 0) {
        char str[1024];
        sprintf(str, "PID: %d\ntime: %ld\nstring: %s\n", getpid(), time(NULL), argv[1]);
        fd = open(FIFOFILE, O_WRONLY);
        if (fd == -1) {
            fprintf(stderr, "file open error\n");
            return -1;
        }
        write(fd, str, strlen(str));
        wait(NULL);
    }
    else if (cpid == 0) {
        sleep(6);
        char msg[1025];
        fd = open(FIFOFILE, O_RDONLY);
        if (fd == -1) {
            fprintf(stderr, "file open error\n");
            return -1;
        }
        int readNum = read(fd, msg, 1024);
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
    remove(FIFOFILE);
}
