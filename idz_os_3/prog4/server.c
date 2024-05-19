#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_QUEUE_LENGTH 5
#define BUFFER_SIZE 1024

typedef struct {
    int sockfd;
    int queue[MAX_QUEUE_LENGTH];
    int queue_length;
    pthread_mutex_t lock;
} Cashier;

Cashier cashier1, cashier2;

void *handle_cashier(void *arg) {
    Cashier *cashier = (Cashier *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        pthread_mutex_lock(&cashier->lock);
        if (cashier->queue_length > 0) {
            int customer = cashier->queue[0];
            // Shift queue
            for (int i = 1; i < cashier->queue_length; i++) {
                cashier->queue[i - 1] = cashier->queue[i];
            }
            cashier->queue_length--;
            pthread_mutex_unlock(&cashier->lock);

            // Process customer
            snprintf(buffer, BUFFER_SIZE, "Processing customer %d\n", customer);
            send(cashier->sockfd, buffer, strlen(buffer), 0);
            sleep(5); // Simulate processing time
        } else {
            pthread_mutex_unlock(&cashier->lock);
            usleep(100000); // Sleep for a short while to avoid busy waiting
        }
    }
}

int add_customer_to_queue(Cashier *cashier, int customer) {
    pthread_mutex_lock(&cashier->lock);
    int result = 0;
    if (cashier->queue_length < MAX_QUEUE_LENGTH) {
        cashier->queue[cashier->queue_length++] = customer;
        result = 1;
    }
    pthread_mutex_unlock(&cashier->lock);
    return result;
}

void handle_customer(int customer, int client_socket) {
    int added = 0;
    if (cashier1.queue_length < MAX_QUEUE_LENGTH) {
        added = add_customer_to_queue(&cashier1, customer);
    } 
    if (!added && cashier2.queue_length < MAX_QUEUE_LENGTH) {
        added = add_customer_to_queue(&cashier2, customer);
    }
    if (!added) {
        char message[BUFFER_SIZE];
        snprintf(message, BUFFER_SIZE, "Customer %d left, both queues are full\n", customer);
        send(client_socket, message, strlen(message), 0);
        printf("%s", message);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int port = atoi(argv[1]);
    char buffer[BUFFER_SIZE] = {0};
    int customer_id = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Waiting for cashier 1 to connect...\n");
    if ((cashier1.sockfd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Cashier 1 connected\n");
    pthread_mutex_init(&cashier1.lock, NULL);
    pthread_t cashier1_thread;
    pthread_create(&cashier1_thread, NULL, handle_cashier, &cashier1);

    printf("Waiting for cashier 2 to connect...\n");
    if ((cashier2.sockfd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Cashier 2 connected\n");
    pthread_mutex_init(&cashier2.lock, NULL);
    pthread_t cashier2_thread;
    pthread_create(&cashier2_thread, NULL, handle_cashier, &cashier2);

    printf("Waiting for customers to connect...\n");
    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0) {
        read(new_socket, buffer, BUFFER_SIZE);
        printf("Received customer %d\n", customer_id);
        handle_customer(customer_id++, new_socket);
        close(new_socket);
    }

    pthread_join(cashier1_thread, NULL);
    pthread_join(cashier2_thread, NULL);

    close(server_fd);
    return 0;
}
