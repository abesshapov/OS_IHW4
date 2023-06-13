#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("IP-address, port, and number of students are needed.");
        exit(1);
    }
    struct sockaddr_in server_addr, client_addr, observer_addr;
    int server_socket, observer_socket;
    int students_number = atoi(argv[3]);
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    observer_socket = socket(AF_INET, SOCK_DGRAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    int observerEnable = 1;
    setsockopt(observer_socket, SOL_SOCKET, SO_BROADCAST, &observerEnable, sizeof(observerEnable));
    memset(&observer_addr, 0, sizeof(observer_addr));
    observer_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "255.255.255.255", &observer_addr.sin_addr);
    observer_addr.sin_port = htons(40000);
    bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    socklen_t client_addr_len = sizeof(client_addr);
    for (int i = 1; i <= students_number; i++) {
        char init_msg;
        recvfrom(server_socket, &init_msg, sizeof(init_msg), 0, (struct sockaddr *) &client_addr, &client_addr_len);                      
        int number = rand() % 100;
        printf("Ticket given to student %d: name a number bigger than %d\n", i, number);
        sendto(server_socket, &number, sizeof(number), 0, (struct sockaddr *) &client_addr, client_addr_len);
        int said_number;
        recvfrom(server_socket, &said_number, sizeof(said_number), 0, (struct sockaddr *) &client_addr, client_addr_len);
        char observer_message[100];
        if (said_number > number) {
            sprintf(observer_message, "Student %d says number %d. Answer is correct, 5, dismissed\n", i, said_number);
            sendto(observer_socket, observer_message, strlen(observer_message), 0, (struct sockaddr *) &observer_addr, sizeof(observer_addr));
        } else {
            sprintf(observer_message, "Student %d says number %d. Answer is incorrect, 2, dismissed\n", i, said_number);
            sendto(observer_socket, observer_message, strlen(observer_message), 0, (struct sockaddr *) &observer_addr, sizeof(observer_addr));
        }
    }
    close(server_socket);
    close(observer_socket);
    printf("Exam has ended");
    return 0;
}