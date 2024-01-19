#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Data.h"
#include "Menu.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#pragma pack(1)

#pragma warning(disable:4996) 

#define PORT 9009
#define BUFFER_SIZE 128
#define SERVER_IP "127.0.0.1"
int main() {

	WSADATA wsaData;
	SOCKET publisherSocket, clientSocket;
	struct sockaddr_in serverAddr;

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	
	int iResult = -1;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup failed.\n");
		return 1;
	}

	
	if ((publisherSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Socket creation failed.\n");
		WSACleanup();
		return 1;
	}


	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serverAddr.sin_port = htons(PORT);


	iResult = connect(publisherSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (iResult == SOCKET_ERROR)
	{
		printf("Unable to connect to server.\n");
		closesocket(publisherSocket);
		WSACleanup();
		return 1;
	}
	

	printf("Connected to server.\n");

	while (true) {
		Data data = {};
		data = Menu();
		char* mess = (char*)malloc(sizeof(char*));
		strcpy(mess, "x");
		int result1 = strcmp(data.message, mess);
		if (result1 == 0) {
			closesocket(publisherSocket);
			WSACleanup();
			break;
		}
		printf("Topic: %s\n Type: %s\n Message: %s\n", data.topic, data.type, data.message);
		strcpy(buffer, data.topic);
		strcpy(buffer+6, "|");
		strcpy(buffer+7, data.type);
		strcpy(buffer+10, "|");
		strcpy(buffer+11, data.message);
		

		iResult = send(publisherSocket, (char*)&buffer, (int)strlen(buffer), 0);

		if (iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(publisherSocket);
			WSACleanup();
			return 1;
		}

		Sleep(500);
	}

	
	

	

	return 0;
}