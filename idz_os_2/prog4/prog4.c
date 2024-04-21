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
#include <sys/wait.h>

#define MAX_QUEUE_LENGTH 5

typedef struct {
    int queue_length[2]; // Очереди для первого и второго кассиров
    bool cashier_sleeping[2];
} shared_data;

shared_data *shm_data;

int sem_id[2]; // Семафоры для первого и второго кассиров

void cashier_process(int cashier_id) {
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

void customer_process(int customer_id) {
    srand(time(NULL) + customer_id);
    printf("Customer %d is trying to enter the queue.\n", customer_id);
    struct sembuf wait_queue = {customer_id % 2, -1, 0}; // Выбираем семафор для очереди
    semop(sem_id[customer_id % 2], &wait_queue, 1); // Используем семафор для соответствующего кассиру
    if (shm_data->queue_length[customer_id % 2] < MAX_QUEUE_LENGTH) {
        shm_data->queue_length[customer_id % 2]++;
        printf("Customer %d entered the queue for Cashier %d.\n", customer_id, customer_id % 2);
    } else {
        printf("Customer %d left because the queue for Cashier %d is full.\n", customer_id, customer_id % 2);
    }
    struct sembuf signal_queue = {customer_id % 2, 1, 0}; // Выбираем семафор для очереди
    semop(sem_id[customer_id % 2], &signal_queue, 1); // Используем семафор для соответствующего кассиру
    exit(EXIT_SUCCESS);
}

void cleanup(int shm_id) {
    for (int i = 0; i < 2; i++) {
        semctl(sem_id[i], 0, IPC_RMID);
    }
    shmdt(shm_data);
    shmctl(shm_id, IPC_RMID, NULL);
}

void signal_handler(int signum, siginfo_t *info, void *ptr) {
    if (signum == SIGINT) {
        printf("\nReceived SIGINT. Cleaning up resources...\n");
        cleanup(info->si_pid);
        exit(EXIT_SUCCESS);
    }
}

int main() {
    struct sigaction sa;
    sa.sa_sigaction = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;

    sigaction(SIGINT, &sa, NULL);

    int shm_id = shmget(IPC_PRIVATE, sizeof(shared_data), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    shm_data = (shared_data *)shmat(shm_id, NULL, 0);
    if (shm_data == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 2; i++) {
        sem_id[i] = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
        if (sem_id[i] == -1) {
            perror("semget");
            exit(EXIT_FAILURE);
        }

        semctl(sem_id[i], 0, SETVAL, 1);
    }

    pid_t cashier_pid_1 = fork();
    if (cashier_pid_1 == 0) {
        cashier_process(0);
    } else if (cashier_pid_1 < 0) {
        perror("fork");
        cleanup(shm_id);
        exit(EXIT_FAILURE);
    }

    pid_t cashier_pid_2 = fork();
    if (cashier_pid_2 == 0) {
        cashier_process(1);
    } else if (cashier_pid_2 < 0) {
        perror("fork");
        cleanup(shm_id);
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i <= 18; i++) {
        pid_t customer_pid = fork();
        if (customer_pid == 0) {
            customer_process(i);
        } else if (customer_pid < 0) {
            perror("fork");
            cleanup(shm_id);
            exit(EXIT_FAILURE);
        }
    }

    while (wait(NULL) != -1)
        ;

    cleanup(shm_id);

    return 0;
}
