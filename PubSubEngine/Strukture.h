#pragma once
typedef struct Data {
	char* topic;
	char* type;
	char* message;
};

typedef struct Topic {
	char* topic= (char*)malloc(sizeof(char*));
	char* type= (char*)malloc(sizeof(char*));
}TOPIC;

typedef struct Client {
	SOCKET clientSocket;
	HANDLE clientThread;
	TOPIC  clientTopic;
	int num = 0;
	int inUse = 0;
	char* name = (char*)malloc(sizeof(char*));
}ClientINFO;