
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>

#define MAX_SIZE (10 * 1024 * 1024)

struct shared_data {
    int ready;
    int size;
    char data[MAX_SIZE];
};

double get_time(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);

    return t.tv_sec + t.tv_nsec / 1000000000.0;
}

int main(int argc, char *argv[]) {
    key_t key;
    int shmid, size;
    struct shared_data *shm;
    double start, end, elapsed;

    if (argc != 2) {
        printf("Usage: %s <size in bytes>\n", argv[0]);
        return 1;
    }

    size = atoi(argv[1]);

    if (size < 0 || size > MAX_SIZE) {
        fprintf(stderr, "Size must be between 0 and %d bytes.\n",
                MAX_SIZE);
        return 1;
    }

    key = ftok(".", 'A');

    if (key == -1) {
        perror("ftok");
        return 1;
    }

    shmid = shmget(key, sizeof(struct shared_data),
                   IPC_CREAT | 0666);

    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    shm = shmat(shmid, NULL, 0);

    if (shm == (void *) -1) {
        perror("shmat");
        return 1;
    }

    shm->ready = 0;
    shm->size = size;

    memset(shm->data, 'A', size);

    printf("Writer sending %d bytes\n", size);

    start = get_time();
    shm->ready = 1;

    while (shm->ready == 1) {
        usleep(1000);
    }

    end = get_time();
    elapsed = end - start;

    printf("Time: %.6f sec\n", elapsed);
    printf("Latency: %.6f sec\n", elapsed);

    if (elapsed > 0) {
        printf("Throughput: %.2f MB/s\n",
               (size / (1024.0 * 1024.0)) / elapsed);
    }

    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
