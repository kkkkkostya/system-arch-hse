#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port> <cashier_id>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);
    int cashier_id = atoi(argv[3]);
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Send a connection message to the server
    snprintf(buffer, BUFFER_SIZE, "Cashier %d", cashier_id);
    sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    printf("Cashier %d connected to server\n", cashier_id);

    while (1) {
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Cashier %d: %s", cashier_id, buffer);
        } else {
            perror("recvfrom error");
            break;
        }
    }

    close(sockfd);
    return 0;
}
