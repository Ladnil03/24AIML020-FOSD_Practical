#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

#define MAX_SIZE 4096

struct message {
    long type;
    char data[MAX_SIZE];
};

double get_time(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);

    return t.tv_sec + t.tv_nsec / 1000000000.0;
}

int main(int argc, char *argv[]) {
    key_t key;
    int msgid, size, count;
    struct message msg;
    double start, end, elapsed;

    if (argc != 3) {
        printf("Usage: %s <message_size> <count>\n", argv[0]);
        return 1;
    }

    size = atoi(argv[1]);
    count = atoi(argv[2]);

    if (size < 1 || size > MAX_SIZE || count < 1) {
        printf("Invalid message size or count\n");
        return 1;
    }

    key = ftok(".", 'A');

    if (key == -1) {
        perror("ftok");
        return 1;
    }

    msgid = msgget(key, IPC_CREAT | 0666);

    if (msgid == -1) {
        perror("msgget");
        return 1;
    }

    msg.type = 1;
    memset(msg.data, 'A', size);

    printf("Sender: sending %d messages of %d bytes\n",
           count, size);

    start = get_time();

    for (int i = 0; i < count; i++) {
        if (msgsnd(msgid, &msg, size, 0) == -1) {
            perror("msgsnd");
            return 1;
        }
    }

    end = get_time();
    elapsed = end - start;

    printf("Time: %.6f sec\n", elapsed);
    printf("Average send time: %.6f sec\n", elapsed / count);

    if (elapsed > 0) {
        printf("Throughput: %.2f MB/s\n",
               ((double) size * count / (1024.0 * 1024.0))
               / elapsed);
    }

    return 0;
}
