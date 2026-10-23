
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_SIZE 4096

struct message {
    long type;
    char data[MAX_SIZE];
};

int main(int argc, char *argv[]) {
    key_t key;
    int msgid, count;
    ssize_t received;
    struct message msg;

    if (argc != 2) {
        printf("Usage: %s <count>\n", argv[0]);
        return 1;
    }

    count = atoi(argv[1]);

    if (count < 1) {
        printf("Invalid message count\n");
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

    printf("Receiver: waiting for messages...\n");

    for (int i = 0; i < count; i++) {
        received = msgrcv(msgid, &msg, MAX_SIZE, 0, 0);

        if (received == -1) {
            perror("msgrcv");
            return 1;
        }

        if (i == 0) {
            printf("Received message type: %ld\n", msg.type);
            printf("Received bytes: %zd\n", received);
            printf("First 10 bytes: ");

            for (int j = 0; j < 10 && j < received; j++) {
                printf("%c", msg.data[j]);
            }

            printf("\n");
        }
    }

    printf("Receiver: received %d messages\n", count);

    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        return 1;
    }

    printf("Message queue removed\n");

    return 0;
}
