#pragma once
#include <ws2tcpip.h>
#include <stdio.h>
#include "Strukture.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

DWORD WINAPI Subscribe(LPVOID lpParam);

DWORD WINAPI ReceiveMessage(LPVOID lpParam);