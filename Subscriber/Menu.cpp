#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include "Menu.h"
#include "Strukture.h"
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

Topic Menu() {

	Topic theme = {};
	theme.topic = (char*)malloc(sizeof(char*));
	theme.type = (char*)malloc(sizeof(char*));

	printf("MENI:\n\n");
	printf("1.ANALOG\n");
	printf("2.STATUS\n");
	char izbor = 'c';
	int broj = 0;
	int broj1 = 0;
	do {
		scanf("%c", &izbor);
		broj = atoi(&izbor);
		switch (broj) {
		case 1:
			strcpy(theme.topic, "ANALOG");
			strcpy(theme.type, "MER");
			break;
		case 2:
			strcpy(theme.topic, "STATUS");
			printf("Izaberi tip:\n");
			printf("1. SWG (switchgear)\n");
			printf("2. CRB (circuit breaker)\n");
			do {
				char izbor1 = 'c';
				scanf("%c", &izbor);
				broj1 = atoi(&izbor);
				if (broj1 < 1 || broj1>2) {
					printf("Molimo unesite vrijednost 1 ili 2\n");
				}
				switch (broj1) {
				case 1:
					strcpy(theme.type, "SWG");
					break;
				case 2:
					strcpy(theme.type, "CRB");
					break;
				}
			} while (broj1 < 1 || broj1>2);
			break;

		}
	} while (broj < 1 || broj>2);
	return theme;
}