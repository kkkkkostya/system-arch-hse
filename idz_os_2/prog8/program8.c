#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>

#define MAX_QUEUE_LENGTH 5
#define SHM_KEY 1234
#define SEM_KEY 5678

typedef struct {
    int queue_length[2]; // Очереди для первого и второго кассиров
    bool cashier_sleeping[2];
} shared_data;

int sem_id;

void cashier_process(int cashier_id, shared_data *shm_data) {
    while (true) {
        if (shm_data->queue_length[cashier_id] == 0) {
            printf("Cashier %d is sleeping.\n", cashier_id);
            shm_data->cashier_sleeping[cashier_id] = true;
            while (shm_data->queue_length[cashier_id] == 0)
                ;
            printf("Cashier %d woke up.\n", cashier_id);
            shm_data->cashier_sleeping[cashier_id] = false;
        }

        printf("Cashier %d is serving a customer.\n", cashier_id);
        shm_data->queue_length[cashier_id]--;
        sleep(1);
    }
}

void customer_process(int customer_id, shared_data *shm_data) {
    srand(time(NULL) + customer_id);
    printf("Customer %d is trying to enter the queue.\n", customer_id);
    struct sembuf wait_queue = {customer_id % 2, -1, 0}; // Выбираем семафор для очереди
    semop(sem_id, &wait_queue, 1); // Используем семафор для соответствующего кассиру
    if (shm_data->queue_length[customer_id % 2] < MAX_QUEUE_LENGTH) {
        shm_data->queue_length[customer_id % 2]++;
        printf("Customer %d entered the queue for Cashier %d.\n", customer_id, customer_id % 2);
    } else {
        printf("Customer %d left because the queue for Cashier %d is full.\n", customer_id, customer_id % 2);
    }
    struct sembuf signal_queue = {customer_id % 2, 1, 0}; // Выбираем семафор для очереди
    semop(sem_id, &signal_queue, 1); // Используем семафор для соответствующего кассиру
    exit(EXIT_SUCCESS);
}

void cleanup(int shm_id, shared_data *shm_data) {
    semctl(sem_id, 0, IPC_RMID);
    shmdt(shm_data);
    shmctl(shm_id, IPC_RMID, NULL);
}

void signal_handler(int signum) {
    if (signum == SIGINT) {
        printf("\nReceived SIGINT. Cleaning up resources...\n");
        exit(EXIT_SUCCESS);
    }
}

int main() {
    signal(SIGINT, signal_handler);

    int shm_id = shmget(SHM_KEY, sizeof(shared_data), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    shared_data *shm_data = (shared_data *)shmat(shm_id, NULL, 0);
    if (shm_data == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    sem_id = semget(SEM_KEY, 2, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget");
        cleanup(shm_id, shm_data);
        exit(EXIT_FAILURE);
    }

    semctl(sem_id, 0, SETVAL, 1);
    semctl(sem_id, 1, SETVAL, 1);

    srand(time(NULL));
    int process_type = rand() % 2; // Randomly choose process type: 0 for cashier, 1 for customer

    if (process_type == 0) {
        int cashier_id = rand() % 2; // Randomly choose cashier id: 0 or 1
        cashier_process(cashier_id, shm_data);
    } else {
        int customer_id = rand() % 100 + 1; // Random customer id between 1 and 100
        customer_process(customer_id, shm_data);
    }

    return 0;
}
