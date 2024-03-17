#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define BUFFER_SIZE 5000

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Использование: %s входной_файл выходной_файл N1 N2\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *input_file = argv[1];
    char *output_file = argv[2];
    int N1 = atoi(argv[3]);
    int N2 = atoi(argv[4]);

    // Открытие входного файла
    int fd_in = open(input_file, O_RDONLY);
    if (fd_in == -1) {
        perror("Не удалось открыть входной файл");
        exit(EXIT_FAILURE);
    }

    // Создание и открытие именованного канала для записи
    const char *fifo_name = "myfifo.fifo";
    if (mkfifo(fifo_name, 0666) == -1) {
        perror("Не удалось создать именованный канал");
        close(fd_in);
        exit(EXIT_FAILURE);
    }
    int fd_fifo = open(fifo_name, O_WRONLY);
    if (fd_fifo == -1) {
        perror("Не удалось открыть именованный канал");
        close(fd_in);
        exit(EXIT_FAILURE);
    }

    // Чтение данных из файла и запись их в именованный канал
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(fd_in, buffer, BUFFER_SIZE)) > 0) {
        if (write(fd_fifo, buffer, bytes_read) != bytes_read) {
            perror("Ошибка записи в именованный канал");
            close(fd_in);
            close(fd_fifo);
            exit(EXIT_FAILURE);
        }
    }

    // Закрытие файлов и канала
    close(fd_in);
    close(fd_fifo);
    // Запуск второго процесса для обработки данных
    execlp("./compile2", "compile2", fifo_name, output_file, argv[3], argv[4], NULL);

    // Удаление именованного канала
    unlink(fifo_name);

    return EXIT_SUCCESS;
}
