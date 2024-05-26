// customer.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    int customer_id = 1;

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

    socklen_t addr_len = sizeof(serv_addr);

    while (1) {
        snprintf(buffer, BUFFER_SIZE, "%d", customer_id);
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, addr_len);

        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
        buffer[n] = '\0';
        printf("Server: %s\n", buffer);

        customer_id++;
        sleep(2); // Simulate customer arrival time
    }

    close(sockfd);
    return 0;
}
