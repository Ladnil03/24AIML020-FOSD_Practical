
#include <stdio.h>

#define P 5
#define R 3

int allocation[P][R];
int max[P][R];
int need[P][R];
int available[R];

void calculateNeed(void) {
    for (int i = 0; i < P; i++)
        for (int j = 0; j < R; j++)
            need[i][j] = max[i][j] - allocation[i][j];
}

int isSafe(void) {
    int work[R];
    int finish[P] = {0};
    int safeSeq[P];
    int count = 0;

    for (int j = 0; j < R; j++)
        work[j] = available[j];

    while (count < P) {
        int found = 0;

        for (int i = 0; i < P; i++) {
            if (finish[i])
                continue;

            int canFinish = 1;

            for (int j = 0; j < R; j++) {
                if (need[i][j] > work[j]) {
                    canFinish = 0;
                    break;
                }
            }

            if (canFinish) {
                for (int j = 0; j < R; j++)
                    work[j] += allocation[i][j];

                safeSeq[count++] = i;
                finish[i] = 1;
                found = 1;
            }
        }

        if (!found)
            break;
    }

    if (count == P) {
        printf("System is in SAFE state.\n");
        printf("Safe sequence: ");

        for (int i = 0; i < P; i++)
            printf("P%d%s", safeSeq[i],
                   i == P - 1 ? "\n" : " -> ");

        return 1;
    }

    printf("System is in UNSAFE state.\n");
    return 0;
}

void requestResources(void) {
    int pid, request[R];

    printf("\nEnter process number (0-%d): ", P - 1);
    scanf("%d", &pid);

    if (pid < 0 || pid >= P) {
        printf("Invalid process number.\n");
        return;
    }

    printf("Enter request for %d resource types: ", R);
    for (int j = 0; j < R; j++)
        scanf("%d", &request[j]);

    for (int j = 0; j < R; j++) {
        if (request[j] < 0) {
            printf("Invalid negative request.\n");
            return;
        }

        if (request[j] > need[pid][j]) {
            printf("Error: request exceeds maximum declared need.\n");
            return;
        }

        if (request[j] > available[j]) {
            printf("Resources unavailable. Process must wait.\n");
            return;
        }
    }

    /* Temporarily allocate requested resources. */
    for (int j = 0; j < R; j++) {
        available[j] -= request[j];
        allocation[pid][j] += request[j];
        need[pid][j] -= request[j];
    }

    printf("\nChecking resulting state...\n");

    if (isSafe()) {
        printf("Request GRANTED.\n");
    } else {
        /* Roll back the tentative allocation. */
        for (int j = 0; j < R; j++) {
            available[j] += request[j];
            allocation[pid][j] -= request[j];
            need[pid][j] += request[j];
        }

        printf("Request DENIED: allocation rolled back.\n");
    }
}

int main(void) {
    printf("Banker's Algorithm\n");
    printf("Enter Allocation matrix (%d x %d):\n", P, R);

    for (int i = 0; i < P; i++)
        for (int j = 0; j < R; j++)
            scanf("%d", &allocation[i][j]);

    printf("Enter Maximum matrix (%d x %d):\n", P, R);

    for (int i = 0; i < P; i++)
        for (int j = 0; j < R; j++)
            scanf("%d", &max[i][j]);

    printf("Enter Available resources (%d values):\n", R);

    for (int j = 0; j < R; j++)
        scanf("%d", &available[j]);

    for (int i = 0; i < P; i++) {
        for (int j = 0; j < R; j++) {
            if (allocation[i][j] < 0 ||
                max[i][j] < allocation[i][j]) {
                printf("Invalid allocation or maximum claim.\n");
                return 1;
            }
        }
    }

    for (int j = 0; j < R; j++) {
        if (available[j] < 0) {
            printf("Invalid available resource count.\n");
            return 1;
        }
    }

    calculateNeed();

    printf("\nNeed matrix:\n");
    for (int i = 0; i < P; i++) {
        for (int j = 0; j < R; j++)
            printf("%d ", need[i][j]);
        printf("\n");
    }

    printf("\nInitial safety check:\n");
    isSafe();

    requestResources();

    return 0;
}
