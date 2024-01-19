#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <locale.h>
#include "Strukture.h"
#include <signal.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#pragma pack(1)

#pragma warning(disable:4996) 
#define SUB_PORT 9000
#define PUB_PORT 9009
#define BUFFER_SIZE 128
#define SERVER_IP "127.0.0.1"
#define MAX_SIZE 21

int exitRequested = 0;


BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType) {
	if (dwCtrlType == CTRL_C_EVENT) {
		printf("Received Ctrl+C. Cleaning up and exiting...\n");
		exitRequested = 1;
		return TRUE;  // Indicate that the signal has been handled
	}

	return FALSE;  // Pass the signal to the next handler
}

int numPublisherClients = 0;


int numSubscriberClients = 0;

HANDLE mutex;

ClientINFO publisherClients[MAX_SIZE];
ClientINFO subscriberClients[MAX_SIZE];

char* old_addr= (char*)malloc(sizeof(char*));
char* new_addr= (char*)malloc(sizeof(char*));
int pub_port = 0;
int sub_port = 0;
//metodu za obradu klijenata
unsigned __stdcall HandleClient(void* clientInfo) {
	ClientINFO* client = (ClientINFO*)clientInfo;

	char buffer[1024];
	int bytesRead;

	while (1) {
		
		bytesRead = recv(client->clientSocket, buffer, sizeof(buffer), 0);
		if (bytesRead > 0) {
			
			
			
			buffer[bytesRead] = '\0';
			char* topic = (char*)malloc(sizeof(char*));
			char* type = (char*)malloc(sizeof(char*));
			char* message = (char*)malloc(sizeof(char*));
			//printf("Received from client: %s\n", buffer);


			if (client->clientSocket == publisherClients[client->num].clientSocket) {
				// Handle publisher data
				int iResult = 0;
				int br = 0;
				for (int i = 0;i < 6;i++) {
					topic[i] = buffer[i];
				}
				topic[6] = '\0';
				for (int i = 7;i < 10;i++) {
					type[br] = buffer[i];
					br++;
				}
				type[br] = '\0';
				br = 0;
				for (int i = 11;i < strlen(buffer);i++) {
					message[br] = buffer[i];
					br++;
				}
				message[br] = '\0';



				printf("\n%s je poslao poruku %s\n", publisherClients[client->num].name, message);

				for (int i = 0;i < numSubscriberClients + 1;i++) {
					if (buffer[0] == 'A') {
						if (subscriberClients[i].clientTopic.topic[0] == 'A') {
							iResult = send(subscriberClients[i].clientSocket, message, (int)strlen(message), 0);
							Sleep(1000);
						}
					}
					else {
						if (subscriberClients[i].clientTopic.topic[0] == 'S') {
							iResult = send(subscriberClients[i].clientSocket, message, (int)strlen(message), 0);
							Sleep(1000);
						}
					}
				}
			}
			else if (client->clientSocket == subscriberClients[client->num].clientSocket) {

				int br = 0;
				for (int i = 0;i < 6;i++) {
					topic[i] = buffer[i];
				}
				topic[6] = '\0';
				for (int i = 7;i < 10;i++) {
					type[br] = buffer[i];
					br++;
				}
				type[br] = '\0';
				printf("\n%s se pretplatio na topic %s, tip %s\n", subscriberClients[client->num].name, topic, type);
				if (buffer[0] == 'A') {

					strcpy(subscriberClients[client->num].clientTopic.topic, "ANALOG");
					strcpy(subscriberClients[client->num].clientTopic.type, "MER");
				}
				else {
					strcpy(subscriberClients[client->num].clientTopic.topic, "STATUS");
					if (buffer[7] == 'S') {
						strcpy(subscriberClients[client->num].clientTopic.type, "SWG");
					}
					else {
						strcpy(subscriberClients[client->num].clientTopic.type, "CRB");
					}
				}
			}
		
		}
		else if (bytesRead == 0) {
			
			if(client->clientSocket == subscriberClients[client->num].clientSocket)
				printf("\n%s is disconnected\n", subscriberClients[client->num].name);
			else
				printf("\n%s is disconnected\n", publisherClients[client->num].name);
			// Cleanup resources
			closesocket(client->clientSocket);
			CloseHandle(client->clientThread);

			client->inUse = 0;

			// Release the mutex
			ReleaseMutex(mutex);
			break;
		}
		else {
			
			Sleep(100);
			continue;
		}
		
	}

}

void SetNonBlockingMode(SOCKET socket) {
	unsigned long nonBlocking = 1;
	ioctlsocket(socket, FIONBIO, &nonBlocking);
}

int main() {

	SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);

	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		fprintf(stderr, "Failed to initialize Winsock\n");
		return 1;
	}

	mutex = CreateMutex(NULL, FALSE, NULL);

	// Create socket for publisher connections
	SOCKET publisherSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (publisherSocket == INVALID_SOCKET) {
		fprintf(stderr, "Failed to create publisher socket\n");
		WSACleanup();
		return 1;
	}

	// Create socket for subscriber connections
	SOCKET subscriberSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (subscriberSocket == INVALID_SOCKET) {
		fprintf(stderr, "Failed to create subscriber socket\n");
		closesocket(publisherSocket);
		WSACleanup();
		return 1;
	}

	struct sockaddr_in publisherAddress;
	publisherAddress.sin_family = AF_INET;
	publisherAddress.sin_port = htons(PUB_PORT);
	publisherAddress.sin_addr.s_addr = INADDR_ANY;
	int AddrSize = sizeof(struct sockaddr_in);
	struct sockaddr_in subscriberAddress;
	subscriberAddress.sin_family = AF_INET;
	subscriberAddress.sin_port = htons(SUB_PORT);
	subscriberAddress.sin_addr.s_addr = INADDR_ANY;

	if (bind(publisherSocket, (struct sockaddr*)&publisherAddress, sizeof(publisherAddress)) == SOCKET_ERROR ||
		bind(subscriberSocket, (struct sockaddr*)&subscriberAddress, sizeof(subscriberAddress)) == SOCKET_ERROR) {
		fprintf(stderr, "Failed to bind sockets\n");
		closesocket(publisherSocket);
		closesocket(subscriberSocket);
		WSACleanup();
		return 1;
	}

	if (listen(publisherSocket, 5) == SOCKET_ERROR || listen(subscriberSocket, 5) == SOCKET_ERROR) {
		fprintf(stderr, "Failed to listen\n");
		closesocket(publisherSocket);
		closesocket(subscriberSocket);
		WSACleanup();
		return 1;
	}

	printf("Server is listening on ports %d (publishers) and %d (subscribers)\n", PUB_PORT, SUB_PORT);

	SetNonBlockingMode(publisherSocket);
	SetNonBlockingMode(subscriberSocket);
	//strcpy(old_addr, "0.0.0.0");
	//strcpy(new_addr, "0.0.0.0");
	pub_port = 9009;
	sub_port = 9000;
	while (!exitRequested) {
		SOCKET publisherClientSocket = accept(publisherSocket, (struct sockaddr*)&publisherAddress, &AddrSize);
		if (ntohs(publisherAddress.sin_port)!=pub_port) {
			printf("\nNew client request accepted. Client address: %s : %d\n", inet_ntoa(publisherAddress.sin_addr), ntohs(publisherAddress.sin_port));
			if (publisherClientSocket != INVALID_SOCKET) {
				WaitForSingleObject(mutex, INFINITE);

				int i;
				for (i = 0; i < MAX_SIZE; ++i) {
					if (!publisherClients[i].inUse) {
						break;
					}
				}

				if (i < MAX_SIZE) {
					//pronadji slobodno mjesto
					publisherClients[i].clientSocket = publisherClientSocket;
					publisherClients[i].clientThread = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)HandleClient,
						&publisherClients[i], 0, NULL);
					publisherClients[i].inUse = 1;
					publisherClients[i].num = numPublisherClients;
					strcpy(publisherClients[i].name, "Publisher");
					char str[1];
					sprintf(str, "%d", publisherClients[i].num);
					strcat(publisherClients[i].name, str);
					numPublisherClients++;
				}
				pub_port = ntohs(publisherAddress.sin_port);
				ReleaseMutex(mutex);
			}

		}
		SOCKET subscriberClientSocket = accept(subscriberSocket, (struct sockaddr*)&subscriberAddress, &AddrSize);
		if (ntohs(subscriberAddress.sin_port)!= sub_port){
			printf("\nNew client request accepted. Client address: %s : %d\n", inet_ntoa(subscriberAddress.sin_addr), ntohs(subscriberAddress.sin_port));

			if (subscriberClientSocket != INVALID_SOCKET) {
				WaitForSingleObject(mutex, INFINITE);

				subscriberClients[numSubscriberClients].clientSocket = subscriberClientSocket;
				subscriberClients[numSubscriberClients].num = numSubscriberClients;
				subscriberClients[numSubscriberClients].clientThread = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)HandleClient,
					&subscriberClients[numSubscriberClients], 0, NULL);
				strcpy(subscriberClients[numSubscriberClients].name, "Subscriber");
				char str[1];
				sprintf(str, "%d", subscriberClients[numSubscriberClients].num);
				strcat(subscriberClients[numSubscriberClients].name, str);
				numSubscriberClients++;
				sub_port = ntohs(subscriberAddress.sin_port);
				ReleaseMutex(mutex);
			}
		}
	}

	closesocket(publisherSocket);
	closesocket(subscriberSocket);
	WSACleanup();

	return 0;
	
}