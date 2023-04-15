#include <winsock2.h>

void ProcessClient(SOCKET clientSocket) {
    // 11. Receive the message from the client and echo it back
    char buffer[512];
    int recvBytes;

    do {
        recvBytes = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (recvBytes > 0) {
            send(clientSocket, buffer, recvBytes, 0);
        }
    } while (recvBytes > 0);

    // 12. Close the client socket
    closesocket(clientSocket);
}