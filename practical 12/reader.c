
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define MAX_SIZE (10 * 1024 * 1024)

struct shared_data {
    int ready;
    int size;
    char data[MAX_SIZE];
};

int main(void) {
    key_t key;
    int shmid;
    struct shared_data *shm;

    key = ftok(".", 'A');

    if (key == -1) {
        perror("ftok");
        return 1;
    }

    shmid = shmget(key, sizeof(struct shared_data), 0666);

    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    shm = shmat(shmid, NULL, 0);

    if (shm == (void *) -1) {
        perror("shmat");
        return 1;
    }

    printf("Reader: waiting for data...\n");

    while (shm->ready == 0)
        usleep(1000);

    printf("Reader: received %d bytes\n", shm->size);
    printf("First 10 bytes: ");

    for (int i = 0; i < 10 && i < shm->size; i++)
        printf("%c", shm->data[i]);

    printf("\n");

    shm->ready = 0;
    shmdt(shm);
    return 0;
}
