#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include "Menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>



#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#pragma pack(1)
#pragma warning(disable:4996)
#pragma warning(suppress : 6387)

Data Menu(){

	 Data data = {};
	
	 data.message = (char*)malloc(sizeof(char*));
	 data.topic = (char*)malloc(sizeof(char*));
	 data.type = (char*)malloc(sizeof(char*));

		printf("MENI:\n\n");

		printf("Izaberi temu:\n");
		printf("1.Analog\n");
		printf("2.Status\n");
		printf("3.Disconnect\n");
		char izbor='c';
		int broj = 0;
		int broj1 = 0;
		
		
		do {
			scanf("%c", &izbor);
			broj = atoi(&izbor);
			switch (broj) {
			case 1:
				strcpy(data.topic, "ANALOG");
				strcpy(data.type, "MER");
				printf("Unesite poruku:\n");
				scanf("%s", data.message);
				break;
			case 2:
				strcpy(data.topic, "STATUS");
				printf("Izaberi tip poruke:\n");
				printf("1. SWG (switchgear)\n");
				printf("2. CRB (circuit breaker)\n");
				do {
					scanf("%c", &izbor);
					broj1 = atoi(&izbor);
					if (broj1 > 2 || broj1 <= 0) {
						printf("Morate unijeti 1 ili 2\n");
					}
				} while (broj1 != 1 && broj1 != 2);
				if (broj1 == 1) {
					strcpy(data.type, "SWG");
				}
				else {
					strcpy(data.type, "CRB");
				}
				printf("Unesite poruku:\n");
				scanf("%s", data.message);
				break;
			case 3:
				strcpy(data.message, "x");
				break;
			}
		} while (broj < 1 || broj > 3);
	return data;
}