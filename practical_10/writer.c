
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#define FIFO_PATH "/tmp/log_fifo"
#define BUFFER_SIZE 4096

double get_time(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1000000000.0;
}

int main(int argc, char *argv[]) {
    int fd, count, size;
    char buffer[BUFFER_SIZE];
    double start, end, elapsed;

    if (argc != 3) {
        printf("Usage: %s <message_size> <count>\n", argv[0]);
        return 1;
    }

    size = atoi(argv[1]);
    count = atoi(argv[2]);

    if (size < 1 || size > BUFFER_SIZE || count < 1) {
        printf("Invalid message size or count\n");
        return 1;
    }

    if (mkfifo(FIFO_PATH, 0666) == -1) {
        if (access(FIFO_PATH, F_OK) == -1) {
            perror("mkfifo");
            return 1;
        }
    }

    fd = open(FIFO_PATH, O_WRONLY);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    memset(buffer, 'A', size);

    printf("Writer: sending %d messages of %d bytes\n",
           count, size);

    start = get_time();

    for (int i = 0; i < count; i++) {
        if (write(fd, buffer, size) != size) {
            perror("write");
            close(fd);
            return 1;
        }
    }

    end = get_time();
    elapsed = end - start;

    printf("Time: %.6f sec\n", elapsed);
    printf("Average write time: %.6f sec\n", elapsed / count);

    if (elapsed > 0) {
        printf("Throughput: %.2f MB/s\n",
               ((double) size * count / (1024.0 * 1024.0))
               / elapsed);
    }

    close(fd);
    return 0;
}
