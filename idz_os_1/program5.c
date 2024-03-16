#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <cstddef>
#include <sys/wait.h>

#define BUFFER_SIZE 5000

// Функция для чтения данных из файла и их передача
void read_from_file_and_send(char *input_file, const char* fifo_name) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    int fd_in = open(input_file, O_RDONLY);
    if (fd_in == -1) {
        perror("Failed to open input file");
        exit(EXIT_FAILURE);
    }

    int fd_fifo = open(fifo_name, O_WRONLY);
    if (fd_fifo == -1) {
        perror("Failed to open fifo");
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = read(fd_in, buffer, BUFFER_SIZE)) > 0) {
        if (write(fd_fifo, buffer, bytes_read) != bytes_read) {
            perror("Write error");
            exit(EXIT_FAILURE);
        }
    }

    close(fd_in);
    close(fd_fifo);
}

// Обработка данных
void process_data_and_send(const char *fifo_in, const char *fifo_out, int start_index, int end_index) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    int fd_fifo_in = open(fifo_in, O_RDONLY);
    if (fd_fifo_in == -1) {
        perror("Failed to open fifo_in");
        exit(EXIT_FAILURE);
    }

    int fd_fifo_out = open(fifo_out, O_WRONLY);
    if (fd_fifo_out == -1) {
        perror("Failed to open fifo_out");
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = read(fd_fifo_in, buffer, BUFFER_SIZE)) > 0) {
        if (bytes_read<end_index){
            end_index = bytes_read;
        }
        for (int i = start_index; i < (start_index + end_index) / 2; i++) {
            char temp = buffer[i];
            buffer[i] = buffer[start_index + end_index - 1 - i];
            buffer[start_index + end_index - 1 - i] = temp;
        }
        if (write(fd_fifo_out, buffer, bytes_read) != bytes_read) {
            perror("Write error");
            exit(EXIT_FAILURE);
        }
    }

    close(fd_fifo_in);
    close(fd_fifo_out);
}

// Запись в файл
void write_to_file(const char *fifo_name, char *output_file) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    int fd_fifo = open(fifo_name, O_RDONLY);
    if (fd_fifo == -1) {
        perror("Failed to open fifo");
        exit(EXIT_FAILURE);
    }

    int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out == -1) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = read(fd_fifo, buffer, BUFFER_SIZE)) > 0) {
        if (write(fd_out, buffer, bytes_read) != bytes_read) {
            perror("Write error");
            exit(EXIT_FAILURE);
        }
    }

    close(fd_fifo);
    close(fd_out);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Need: %s input_file output_file start_index end_index\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *input_file = argv[1];
    char *output_file = argv[2];
    const char *fifo1 = "first.fifo";
    const char *fifo2 = "second.fifo";
    int start_index = atoi(argv[3]);
    int end_index = atoi(argv[4]);

    mkfifo(fifo1, 0666);
    mkfifo(fifo2, 0666);

    if (fork() == 0) {
        read_from_file_and_send(input_file, fifo1);
        exit(EXIT_SUCCESS);
    }

    if (fork() == 0) {
        process_data_and_send(fifo1, fifo2, start_index, end_index);
        exit(EXIT_SUCCESS);
    }
    if (fork() == 0) {
        write_to_file(fifo2, output_file);
        exit(EXIT_SUCCESS);
    }

    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }
    unlink(fifo1);
    unlink(fifo2);

    return 0;
}
