#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>

#define BUFFER_SIZE 5000

// Функция для чтения данных из файла и передачи
void read_from_file_and_send(int fd_out,char name[]) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    int fd_in = open(name, O_RDONLY);
    if (fd_in == -1) {
        perror("Failed to open input file");
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = read(fd_in, buffer, BUFFER_SIZE)) > 0) {
        if (write(fd_out, buffer, bytes_read) != bytes_read) {
            perror("Write error");
            exit(EXIT_FAILURE);
        }
    }

    close(fd_in);
    close(fd_out);
}

// Функция для обработки данных и передачи результата
void process_data_and_send(int fd_in, int fd_out, int start_index, int end_index) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(fd_in, buffer, BUFFER_SIZE)) > 0) {
        if (bytes_read<end_index){
            end_index = bytes_read;
        }
        for (int i = start_index; i < (start_index + end_index) / 2; i++) {
            char temp = buffer[i];
            buffer[i] = buffer[start_index + end_index - 1 - i];
            buffer[start_index + end_index - 1 - i] = temp;
        }

        if (write(fd_out, buffer, bytes_read) != bytes_read) {
            perror("Write error");
            exit(EXIT_FAILURE);
        }
    }

    close(fd_in);
    close(fd_out);
}

// Функция для записи данных в файл
void write_to_file(int fd_in, char out_name[]) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    int fd_out = open(out_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out == -1) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = read(fd_in, buffer, BUFFER_SIZE)) > 0) {
        if (write(fd_out, buffer, bytes_read) != bytes_read) {
            perror("Write error");
            exit(EXIT_FAILURE);
        }
    }

    close(fd_out);
    close(fd_in);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s input_file output_file start_index end_index\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int start_index = atoi(argv[3]);
    int end_index = atoi(argv[4]);
    char inp_name[100] = "";
    char out_name[100] = "";
    strncpy(inp_name, argv[1], sizeof(inp_name) - 1);
    strncpy(out_name, argv[2], sizeof(out_name) - 1);

    int pipe1[2], pipe2[2];
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe error");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("Fork error");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) {
        // Child process 1
        close(pipe1[0]); close(pipe2[0]); close(pipe2[1]);
        read_from_file_and_send(pipe1[1],inp_name);
        exit(EXIT_SUCCESS);
    }

    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("Fork error");
        exit(EXIT_FAILURE);
    } else if (pid2 == 0) {
        // Child process 2
        close(pipe1[1]); close(pipe2[0]);
        process_data_and_send(pipe1[0], pipe2[1], start_index, end_index);
        exit(EXIT_SUCCESS);
    }

    close(pipe1[0]); close(pipe1[1]); close(pipe2[1]);
    write_to_file(pipe2[0],out_name);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
