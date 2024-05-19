#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port> <num_customers>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);
    int num_customers = atoi(argv[3]);

    struct sockaddr_in serv_addr;

    for (int i = 0; i < num_customers; i++) {
        int sockfd;
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Socket creation error");
            exit(EXIT_FAILURE);
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(server_port);

        if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
            perror("Invalid address / Address not supported");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("Connection Failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        char message[BUFFER_SIZE];
        snprintf(message, BUFFER_SIZE, "Customer %d", i + 1);
        send(sockfd, message, strlen(message), 0);

        char response[BUFFER_SIZE];
        int valread = read(sockfd, response, BUFFER_SIZE);
        if (valread > 0) {
            response[valread] = '\0';
            printf("%s", response);
        }

        close(sockfd);
        sleep(1); // Simulate time between customers
    }

    return 0;
}
