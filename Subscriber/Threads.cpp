#include "Threads.h"
#include "Menu.h"
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#pragma pack(1)

#pragma warning(disable:4996) 
#define BUFFER_SIZE 128

DWORD WINAPI Subscribe(LPVOID lpParam) {

	SOCKET clientSocket = *(SOCKET*)lpParam;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	int iResult = -1;

	Topic theme = {};
	theme.topic = (char*)malloc(sizeof(char*));
	theme.type = (char*)malloc(sizeof(char*));

	theme = Menu();
	if (theme.topic[0] == 'D') {
		strcpy(buffer, theme.topic);
		strcpy(buffer + 7, "|");
		strcpy(buffer + 8, theme.type);
	}
	else {
		strcpy(buffer, theme.topic);
		strcpy(buffer + 6, "|");
		strcpy(buffer + 7, theme.type);
	}
	iResult = send(clientSocket, (char*)&buffer, (int)strlen(buffer), 0);

	if (iResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}
}


DWORD WINAPI ReceiveMessage(LPVOID lpParam) {

	SOCKET clientSocket = *(SOCKET*)lpParam;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	int iResult = -1;

	while (true) {
		iResult = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (iResult > 0)
		{
			buffer[iResult] = '\0';
			printf("\nPoruka: %s\n", buffer);
			Sleep(1000);

		}
		else if (iResult == 0)
		{
			printf("Connection with server closed.\n");
			//closesocket(acceptedSocketServer);
		}
		else
		{
			int ierr = WSAGetLastError();
			if (ierr == WSAEWOULDBLOCK) {  
				Sleep(90); 

				continue;
			}

		}
	}

}