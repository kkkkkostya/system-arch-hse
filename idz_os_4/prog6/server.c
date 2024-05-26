#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_QUEUE_LENGTH 5
#define BUFFER_SIZE 1024

typedef struct {
    int queue[MAX_QUEUE_LENGTH];
    int queue_length;
    pthread_mutex_t lock;
    int id;
    struct sockaddr_in addr;
    socklen_t addr_len;
} Cashier;

Cashier cashier1, cashier2;
int server_sockfd;
struct sockaddr_in monitoring_addr;
socklen_t monitoring_addr_len;
int monitoring_connected = 0;

void *handle_cashier(void *arg) {
    Cashier *cashier = (Cashier *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        pthread_mutex_lock(&cashier->lock);
        if (cashier->queue_length > 0) {
            int customer = cashier->queue[0];
            for (int i = 1; i < cashier->queue_length; i++) {
                cashier->queue[i - 1] = cashier->queue[i];
            }
            cashier->queue_length--;
            pthread_mutex_unlock(&cashier->lock);

            snprintf(buffer, BUFFER_SIZE, "Cashier %d is processing customer %d\n", cashier->id, customer);
            sendto(server_sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&cashier->addr, cashier->addr_len);
            if (monitoring_connected) {
                sendto(server_sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&monitoring_addr, monitoring_addr_len);
            }
            printf("%s", buffer);
            sleep(4); // Simulate processing time
        } else {
            pthread_mutex_unlock(&cashier->lock);
            usleep(100000); // Sleep for a short time to prevent busy waiting
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

void handle_customer(int customer, struct sockaddr_in *client_addr, socklen_t addr_len) {
    int added = 0;
    if (cashier1.queue_length < MAX_QUEUE_LENGTH) {
        added = add_customer_to_queue(&cashier1, customer);
    } 
    if (!added && cashier2.queue_length < MAX_QUEUE_LENGTH) {
        added = add_customer_to_queue(&cashier2, customer);
    }

    char message[BUFFER_SIZE];
    if (!added) {
        snprintf(message, BUFFER_SIZE, "Customer %d left, both queues are full\n", customer);
    } else {
        snprintf(message, BUFFER_SIZE, "Customer %d added to queue\n", customer);
    }

    sendto(server_sockfd, message, strlen(message), 0, (struct sockaddr *)client_addr, addr_len);
    if (monitoring_connected) {
        sendto(server_sockfd, message, strlen(message), 0, (struct sockaddr *)&monitoring_addr, monitoring_addr_len);
    }
    printf("%s", message);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    int customer_id = 1;

    if ((server_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(server_sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is up and running...\n");

    pthread_t cashier1_thread, cashier2_thread;
    cashier1.id = 1;
    cashier2.id = 2;
    pthread_mutex_init(&cashier1.lock, NULL);
    pthread_mutex_init(&cashier2.lock, NULL);

    pthread_create(&cashier1_thread, NULL, handle_cashier, &cashier1);
    pthread_create(&cashier2_thread, NULL, handle_cashier, &cashier2);

    while (1) {
        int n = recvfrom(server_sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        buffer[n] = '\0';

        if (strncmp(buffer, "Cashier 1", 9) == 0) {
            cashier1.addr = client_addr;
            cashier1.addr_len = addr_len;
            printf("Cashier 1 connected\n");
        } else if (strncmp(buffer, "Cashier 2", 9) == 0) {
            cashier2.addr = client_addr;
            cashier2.addr_len = addr_len;
            printf("Cashier 2 connected\n");
        } else if (strncmp(buffer, "Monitoring client connected", 27) == 0) {
            monitoring_addr = client_addr;
            monitoring_addr_len = addr_len;
            monitoring_connected = 1;
            printf("Monitoring client connected\n");
        } else {
            int customer = atoi(buffer);
            printf("Received customer %d\n", customer);
            handle_customer(customer_id++, &client_addr, addr_len);
        }
    }

    pthread_join(cashier1_thread, NULL);
    pthread_join(cashier2_thread, NULL);

    close(server_sockfd);
    return 0;
}
