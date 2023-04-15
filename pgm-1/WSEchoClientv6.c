// CS 2690 Program 1 
// Simple Windows Sockets Echo Client (IPv6)
// Last update: 2/12/19
// <Tanner Shimanek> <cs-2690> <3/4/2023>
// <Windows 11, Visual Studio 2022>
//
// Usage: WSEchoClientv6 <server IPv6 address> <server port> <"message to echo">
// Companion server is WSEchoServerv6
// Server usage: WSEchoServerv6 <server port>
//
// This program is coded in conventional C programming style, with the 
// exception of the C++ style comments.
//
// I declare that the following source code was written by me or provided
// by the instructor. I understand that copying source code from any other 
// source or posting solutions to programming assignments (code) on public
// Internet sites constitutes cheating, and that I will receive a zero 
// on this project if I violate this policy.
// ----------------------------------------------------------------------------


#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define RCVBUFSIZ 50

// declare any functions located in other .c files here
void DisplayFatalErr(char* errMsg); // writes error message before abnormal termination

void main(int argc, char* argv[])   // argc is # of strings following command, argv[] is array of ptrs to the strings
{
	// Declare ALL variables and structures for main() HERE, NOT INLINE (including the following...)
	WSADATA wsaData;                // contains details about WinSock DLL implementation
	struct sockaddr_in6 serverInfo;	// standard IPv6 structure that holds server socket info

	int sock;
	int sent;
	int msgLength;
	int output;

	int ip = 0;
	int port = 0;

	int perrno = -1;
	int v6Only = 0;

	unsigned short serverPort;
	char* serverIPaddr;

	int bytesRead;
	int bufferLength;
	int byteTracker;


	// Verify correct number of command line arguments, else do the following:
	if (argc != 4) {
		fprintf(stderr, "Invalid number of arguments, must contain (4).\n");
		exit(1);
	}

	// Retrieve the command line arguments. (Sanity checks not required, but port and IP addr will need
	// to be converted from char to int.  See slides 11-15 & 12-3 for details.)
	__try {
		ip = atoi(argv[1]);
		port = atoi(argv[2]);
	}
	__except (printf("Invalid IP address or Port")) {
		exit(1);
	}

	// Initialize Winsock 2.0 DLL. Returns 0 if ok. If this fails, fprint error message to stderr as above & exit(1).  
	output = WSAStartup(MAKEWORD(2, 0), &wsaData);
	
	if (output != 0) {
		fprintf(stderr, "WSAStartup failed.\n.");
		exit(1);
	}

	// Create an IPv6 TCP stream socket.  Now that Winsock DLL is loaded, we can signal any errors as shown on next line:
	// DisplayFatalErr("socket() function failed.");
	__try {
		sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	}
	__except (INVALID_SOCKET) {
		DisplayFatalErr("Creating socket failed.. ");
	}

	// Display helpful confirmation messages after key socket calls like this:
	printf("Socket created successfully. Press any key to continue.. ");
	(void)getchar();     // needed to hold console screen open
	printf("\n");

	// If doing extra credit IPv4 address handling option, add the setsockopt() call as follows...
	if (perrno = setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&v6Only, sizeof(v6Only)) != 0)
		DisplayFatalErr("setsockopt() function failed.");  

	// Zero out the sockaddr_in6 structure and load server info into it.  See slide 11-15.
	// Don't forget any necessary format conversions.

	serverIPaddr = argv[1];
	serverPort = atoi(argv[2]);
	memset(&serverInfo, 0, sizeof(serverInfo));
	
	// load serverInfo
	serverInfo.sin6_family = AF_INET6;
	serverInfo.sin6_port = htons(serverPort);
		
	// Convert input addr and and load into sockaddr_in6
	inet_pton(AF_INET6, serverIPaddr, &serverInfo.sin6_addr);
	msgLength = strlen(argv[3]);

	// Attempt connection to the server.  If it fails, call DisplayFatalErr() with appropriate message,
	// otherwise printf() confirmation message
	if (connect(sock, (struct sockaddr*)&serverInfo, sizeof(serverInfo)) < 0) {
		DisplayFatalErr("Server connection failed.");
	}
	else {
		printf("Server connection successful..\n");
	}

	// Send message to server (without '\0' null terminator). Check for null msg (length=0) & verify all bytes were sent...
	// ...else call DisplayFatalErr() with appropriate message as before
	sent = send(sock, argv[3], msgLength, 0);

	// Checks both null and verifies that all bits were sent
	if (sent != msgLength) {
		DisplayFatalErr("Message send failed..\n");
	}
	else {
		printf("Listening for message on server..\n");
		char rcvBuffer[RCVBUFSIZ];
		bytesRead = 0;
		bufferLength = (int)strlen(argv[3]);
		byteTracker = 1;

		memset(rcvBuffer, 0, strlen(rcvBuffer));
		printf("Reading message..\n");

		// Retrieve the message returned by server.  Be sure you've read the whole thing (could be multiple segments). 
		// Manage receive buffer to prevent overflow with a big message.
		// Display ALL of the received message, in printable C string format.
		while (bytesRead != msgLength) {
			// Call DisplayFatalErr() if this fails.  (Lots can go wrong here, see slides.)
			byteTracker = recv(sock, rcvBuffer, RCVBUFSIZ - 1, 0);
			if (byteTracker < 0) {
				DisplayFatalErr("Failed receiving message..\n");
			}

			bytesRead += byteTracker;

			if (byteTracker == msgLength) {
				rcvBuffer[strlen(rcvBuffer)] = 0;
				printf(rcvBuffer);
			}
			else if (bytesRead <= msgLength) {
				rcvBuffer[strlen(rcvBuffer)] = 0;
				printf(rcvBuffer);
			}

			memset(rcvBuffer, 0, strlen(rcvBuffer));
		} 
		
		printf("\nNumber of bytes read: %d\n", bytesRead);
	}

	// Close the TCP connection (send a FIN) & print appropriate message.
	closesocket(sock);

	// Release the Winsock DLL
	WSACleanup();

	exit(0);
}
