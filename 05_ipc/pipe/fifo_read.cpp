#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define P_FIFO "/tmp/p_fifo"
int main(int argc, char** argv) {
    char cache[100];
    int  fd;
    memset(cache, 0, sizeof(cache));      /* Initialize memory */
    if (access(P_FIFO, F_OK) == 0) {      /* FIFO file already exists */
        execlp("rm", "-f", P_FIFO, NULL); /* Remove it */
        printf("access.\n");
    }
    if (mkfifo(P_FIFO, 0777) < 0) {
        printf("createnamed pipe failed.\n");
    }
    fd = open(P_FIFO,
        O_RDONLY | O_NONBLOCK); /* Open in non-blocking mode, read-only */
    while (1) {
        memset(cache, 0, sizeof(cache));
        if ((read(fd, cache, 100)) == 0) { /* No data read */
            printf("nodata:\n");
        } else {
            printf("getdata:%s\n", cache); /* Data read, print it out */
        }
        sleep(1); /* Sleep for 1 second */
    }
    close(fd);
    return 0;
}
