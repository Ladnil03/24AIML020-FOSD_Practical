
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define FIFO_PATH "/tmp/log_fifo"
#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    int fd, count;
    ssize_t bytes;
    char buffer[BUFFER_SIZE];

    if (argc != 2) {
        printf("Usage: %s <count>\n", argv[0]);
        return 1;
    }

    count = atoi(argv[1]);

    if (count < 1) {
        printf("Invalid count\n");
        return 1;
    }

    if (mkfifo(FIFO_PATH, 0666) == -1 &&
        access(FIFO_PATH, F_OK) == -1) {
        perror("mkfifo");
        return 1;
    }

    fd = open(FIFO_PATH, O_RDONLY);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("Reader: waiting for log messages...\n");

    for (int i = 0; i < count; i++) {
        bytes = read(fd, buffer, sizeof(buffer));

        if (bytes == -1) {
            perror("read");
            close(fd);
            return 1;
        }

        if (bytes == 0) {
            printf("Writer closed the FIFO early\n");
            close(fd);
            return 1;
        }

        if (i == 0) {
            printf("Received bytes: %zd\n", bytes);
            printf("First 10 bytes: ");

            for (int j = 0; j < 10 && j < bytes; j++) {
                printf("%c", buffer[j]);
            }

            printf("\n");
        }
    }

    printf("Reader: completed %d reads\n", count);

    close(fd);
    return 0;
}
