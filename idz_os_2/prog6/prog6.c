#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <sys/wait.h>

#define MAX_QUEUE_LENGTH 5

typedef struct {
    int queue_length[2]; // Очереди для первого и второго кассиров
    bool cashier_sleeping[2];
} shared_data;

sem_t *semaphores[2]; // Семафоры для первого и второго кассиров
shared_data *shm_data;

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
    sem_wait(semaphores[customer_id % 2]); // Выбираем семафор для очереди
    if (shm_data->queue_length[customer_id % 2] < MAX_QUEUE_LENGTH) {
        shm_data->queue_length[customer_id % 2]++;
        printf("Customer %d entered the queue for Cashier %d.\n", customer_id, customer_id % 2);
    } else {
        printf("Customer %d left because the queue for Cashier %d is full.\n", customer_id, customer_id % 2);
    }
    sem_post(semaphores[customer_id % 2]); // Выбираем семафор для очереди
    exit(EXIT_SUCCESS);
}

void cleanup() {
    for (int i = 0; i < 2; i++) {
        sem_close(semaphores[i]);
    }
    munmap(shm_data, sizeof(shared_data));
}

void signal_handler(int signum) {
    if (signum == SIGINT) {
        printf("\nReceived SIGINT. Cleaning up resources...\n");
        cleanup();
        exit(EXIT_SUCCESS);
    }
}

int main() {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);

    int shm_fd = shm_open("/shm", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    ftruncate(shm_fd, sizeof(shared_data));
    shm_data = (shared_data *)mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_data == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 2; i++) {
        char sem_name[20];
        sprintf(sem_name, "/sem%d", i);
        semaphores[i] = sem_open(sem_name, O_CREAT, 0666, 1);
        if (semaphores[i] == SEM_FAILED) {
            perror("sem_open");
            exit(EXIT_FAILURE);
        }
    }

    pid_t cashier_pid_1 = fork();
    if (cashier_pid_1 == 0) {
        cashier_process(0);
    } else if (cashier_pid_1 < 0) {
        perror("fork");
        cleanup();
        exit(EXIT_FAILURE);
    }

    pid_t cashier_pid_2 = fork();
    if (cashier_pid_2 == 0) {
        cashier_process(1);
    } else if (cashier_pid_2 < 0) {
        perror("fork");
        cleanup();
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i <= 14; i++) {
        pid_t customer_pid = fork();
        if (customer_pid == 0) {
            customer_process(i);
        } else if (customer_pid < 0) {
            perror("fork");
            cleanup();
            exit(EXIT_FAILURE);
        }
    }

    while (wait(NULL) != -1)
        ;

    cleanup();

    return 0;
}
