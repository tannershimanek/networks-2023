#include <stdio.h>       // for print functions
#include <stdlib.h>      // for exit() 
#include <winsock2.h>	 // for Winsock2 functions
#include <ws2tcpip.h>    // adds support for getaddrinfo & getnameinfo for v4+6 name resolution
#include <Ws2ipdef.h>    // optional - needed for MS IP Helper

#pragma comment(lib,"ws2_32.lib") //Winsock Library
#define RCVBUFSIZ 50
#define BUFSIZE 100 // define a constant


// declare any functions located in other .c files here
void DisplayFatalErr(char* errMsg); // writes error message before abnormal termination


int main(int argc, char* argv[])
{
	WSADATA wsaData;                // contains details about WinSock DLL implementation

	struct sockaddr_in6 serverInfo;	// standard IPv6 structure that holds server socket info
	struct sockaddr_in6 clientInfo; // Holds client port & addr after client conn

	int clientInfoLen;
	int clientSock;
	int sock;

	int perrno = -1;
	int v6Only = 0;
	
	unsigned short serverPort;
	int defaultPort = 5000;
	int port;

	int MAXQUEUED = 100;

	if (argc > 2) {
		fprintf(stderr, "Invalid number of arguments, must contain no more than (2).\n");
		exit(1);
	}

	__try {
		if (argc != 2) {
			port = defaultPort;
		}
		else {
			port = atoi(argv[1]);
		}
	}
	__except (printf("Invalid port.")) {
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
		DisplayFatalErr("WSAStartup failed.. ");
		exit(1);
	}

	__try {
		sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	}
	__except (INVALID_SOCKET) {
		DisplayFatalErr("Creating socket failed.. ");
	}

	if (perrno = setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&v6Only, sizeof(v6Only)) != 0)
		DisplayFatalErr("setsockopt() function failed.");

	serverPort = port;
	memset(&serverInfo, 0, sizeof(serverInfo));

	serverInfo.sin6_family = AF_INET6;
	serverInfo.sin6_port = htons(serverPort);
	serverInfo.sin6_addr = in6addr_any;

	//int PASCAL FAR bind(SOCKET s, struct sockaddr FAR * addr, int namelen);
	bind(sock, (struct sockaddr*)&serverInfo, sizeof(serverInfo));

	listen(sock, MAXQUEUED);

	printf("\nTS's IPv6 server is ready for client connection...\n");

	for (;;) {
		clientInfoLen = sizeof(clientInfo); // Do this before call to accept()
		clientSock = accept(sock, (struct sockaddr*)&clientInfo, &clientInfoLen);
		char clientIP[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &clientInfo.sin6_addr, clientIP, sizeof(clientIP));

		if (clientSock == INVALID_SOCKET) {
			DisplayFatalErr("Failed to accept client connection...");
		}
		else {
			printf("Processing the client at %s ", clientIP);
			printf("client port %d, ", ntohs(clientInfo.sin6_port));
			printf("server port %d \n", ntohs(serverInfo.sin6_port));
		}

		ProcessClient(clientSock);

	}

}

