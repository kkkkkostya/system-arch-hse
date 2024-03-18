#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_KEY 0x1111
#define BUFFER_SIZE 10

struct shared_memory {
    int buffer[BUFFER_SIZE];
    int index;
};

int main() {
    int shm_id;
    struct shared_memory *shm_ptr;

    // Создаем разделяемую память
    shm_id = shmget(SHM_KEY, sizeof(struct shared_memory), IPC_CREAT | 0666);
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

    // Генерируем случайные числа и помещаем их в разделяемую память
    srand(getpid());
    shm_ptr->index = 0;
    while (1) {
        if (shm_ptr->index < BUFFER_SIZE) {
            shm_ptr->buffer[shm_ptr->index++] = rand() % 100;
            sleep(1); // Для имитации задержки
        }
    }

    return 0;
}
