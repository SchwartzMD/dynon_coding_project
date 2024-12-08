#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

// Define the data structure
typedef struct {
    int integer;
    float floating;
    char str[50];
    enum { type1, type2, type3 } enumType;
} Data;

// Function to convert enum to string
const char* enumToString(int t) {
    switch (t) {
        case 0: return "type1";
        case 1: return "type2";
        case 2: return "type3";
        default: return "unknown";
    }
}

int main() {
    const char* socketPath = "/tmp/ipc_socket";
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath, sizeof(addr.sun_path) - 1);
    unlink(socketPath); // Remove existing socket file

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Bind failed");
        close(sockfd);
        return 1;
    }

    if (listen(sockfd, 5) == -1) {
        perror("Listen failed");
        close(sockfd);
        return 1;
    }

    printf("Waiting for connections...\n");

    int clientSock = accept(sockfd, NULL, NULL);

    Data data;
    while (recv(clientSock, &data, sizeof(data), 0) > 0) {
        printf("Received: int: %d, float: %.2f, string: %s, enum: %s\n",
               data.integer, data.floating, data.str, enumToString(data.enumType));
    }

    close(clientSock);
    close(sockfd);
    return 0;
}
