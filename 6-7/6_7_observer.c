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
    if (argc != 2) {
        printf("Port is needed.");
        exit(1);
    }
    struct sockaddr_in server_addr;
    int observer_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (observer_socket < 0) {
        printf("Error while creating socket.");
        exit(1);
    }
    int observerEnable = 1;
    setsockopt(observer_socket, SOL_SOCKET, SO_BROADCAST, &observerEnable, sizeof(observerEnable));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    sleep(1);
    int binding = bind(observer_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (binding < 0) {
        perror("Error while binding");
        exit(1);
    }
    char observer_info[4096];
    while (true) {
        int bytes = recv(observer_socket, observer_info, sizeof(observer_info) - 1, 0);
        if (bytes <= 0) {
            break;
        }
        observer_info[bytes] = '\0';
        printf("%s", observer_info);
    }
    close(observer_socket);
    return 0;
}