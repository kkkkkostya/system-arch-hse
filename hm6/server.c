#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

#define SHM_KEY 0x1111
#define BUFFER_SIZE 10

int shm_id;

struct shared_memory {
    int buffer[BUFFER_SIZE];
    int index;
};

void cleanup(int signal) {
    // Удаляем разделяемую память
    shmctl(shm_id, IPC_RMID, NULL);
    exit(0);
}

int main() {
    struct shared_memory *shm_ptr;

    // Получаем доступ к разделяемой памяти
    shm_id = shmget(SHM_KEY, sizeof(struct shared_memory), 0666 | IPC_CREAT);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }

    // Присоединяемся к разделяемой памяти
    shm_ptr = (struct shared_memory *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *) -1) {
        perror("shmat");
        exit(1);
    }

    // Установка обработчика сигнала SIGINT
    signal(SIGINT, cleanup);

    // Основной код сервера для вывода чисел из разделяемой памяти
    while (1) {
        if (shm_ptr->index > 0) {
            printf("Received: %d\n", shm_ptr->buffer[--shm_ptr->index]);
            fflush(stdout);
            sleep(1); // Пауза для имитации задержки
        }
    }

    return 0;
}
