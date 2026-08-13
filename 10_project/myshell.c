#include <arpa/inet.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int  cmdNumber = 2;
    char filename[1024][1024];
    int  fileno = 0;
    // keep the filename
    for (int i = 1; i < argc; i++) {
        strcpy(filename[fileno], argv[i]);
        fileno++;
    }
    int   pipeFd[2];
    pid_t pids[1024];
    int   status           = 0;
    char* cmdArray[32][32] = { { "cat", NULL }, { "wc", "-l", NULL } };
    // step 2: loop every filename
    int fd;
    for (int j = 0; j < fileno; j++) {
        fd = open(filename[j], O_RDONLY);
        // step 3: loop every pipeline command
        for (int i = 0; i < cmdNumber; i++) {
            // cmdArray[0]
            // create the sub process and execute the command
            pipe(pipeFd); // use to connect two command A->B
            pid_t pid = fork();
            if (pid > 0) { // parent
                pids[i] = pid;
                close(pipeFd[1]);
                fd = pipeFd[0]; // keep in temp variable, use in next command
            } else {            // child
                dup2(fd, STDIN_FILENO);
                close(fd);
                dup2(pipeFd[1], STDOUT_FILENO);
                close(pipeFd[1]);
                char** args = cmdArray[i];
                execvp(args[0], args);
            }
        }
        // step 4: reap all children process
        for (int i = 0; i < cmdNumber; i++) {
            waitpid(pids[i], &status, 0);
        }
    }
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    int n = read(fd, buf, sizeof(buf));
    printf("read bytes = %d, value = %s", n, buf);
    return WEXITSTATUS(status);
}
