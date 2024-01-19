#define WIN32_LEAN_AND_MEAN
#include "Menu.h"
#include "Threads.h"
#include "strukture.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <locale.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#pragma pack(1)

#pragma warning(disable:4996) 
#define PORT 9000
#define BUFFER_SIZE 128
#define SERVER_IP "127.0.0.1"



int main() {
	const char* currentLocale = setlocale(LC_ALL, NULL);
	printf("Current Locale: %s\n", currentLocale);


	WSADATA wsaData;
	SOCKET engineSocket, clientSocket;
	struct sockaddr_in serverAddr;

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);

	int iResult = -1;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup failed.\n");
		return 1;
	}

	if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Socket creation failed.\n");
		WSACleanup();
		return 1;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serverAddr.sin_port = htons(PORT);

	iResult = connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (iResult == SOCKET_ERROR)
	{
		printf("Unable to connect to server.\n");
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}
	
	printf("Connected to server.\n");
	struct sockaddr_in localAddress;
	int addrLength = sizeof(localAddress);
	getsockname(clientSocket, (struct sockaddr*)&localAddress, &addrLength);

	char ipString[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &localAddress.sin_addr, ipString, sizeof(ipString));

	printf("Client IP address: %s\n", ipString);

	
	DWORD rec;
	DWORD sub;
	HANDLE hThread = CreateThread(NULL, 0, &Subscribe, &clientSocket, 0, &sub);

	if (hThread != NULL)
	{
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}

	hThread = CreateThread(NULL, 0, &ReceiveMessage, &clientSocket, 0, &rec);

	if (hThread != NULL)
	{
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}

	Sleep(1500);
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}