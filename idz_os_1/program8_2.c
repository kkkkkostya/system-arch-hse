#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define BUFFER_SIZE 5000

void swap_characters(char *buffer, int N1, int N2) {
    if (N1 >= N2) {
        fprintf(stderr, "Ошибка: N1 должен быть меньше N2\n");
        return;
    }

    int length = strlen(buffer);
    if (N2 > length) {
        N2 = length;
    }

    for (int i = N1; i <= (N1 + N2) / 2; i++) {
        char temp = buffer[i];
        buffer[i] = buffer[N1 + N2 - 1 - i];
        buffer[N1 + N2 - 1 - i] = temp;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Использование: %s выходной_файл N1 N2\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *fifo_name = "myfifo.fifo";
    char *output_file = argv[2];
    int N1 = atoi(argv[3]);
    int N2 = atoi(argv[4]);

    // Открытие именованного канала для чтения
    int fd_fifo = open(fifo_name, O_RDONLY);
    if (fd_fifo == -1) {
        perror("Не удалось открыть именованный канал");
        exit(EXIT_FAILURE);
    }

    // Чтение данных из именованного канала
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = read(fd_fifo, buffer, BUFFER_SIZE);
    if (bytes_read == -1) {
        perror("Ошибка чтения из именованного канала");
        close(fd_fifo);
        exit(EXIT_FAILURE);
    }

    // Обработка данных
    swap_characters(buffer, N1, N2);

    // Запись результата в выходной файл
    FILE *output_fp = fopen(output_file, "w");
    if (output_fp == NULL) {
        perror("Не удалось открыть выходной файл");
        close(fd_fifo);
        exit(EXIT_FAILURE);
    }
    fwrite(buffer, 1, strlen(buffer), output_fp);

    // Закрытие файлов и канала
    fclose(output_fp);
    close(fd_fifo);

    return EXIT_SUCCESS;
}
