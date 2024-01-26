//
// Created by floweryclover on 2024-01-25.
//

#include "Wsa.h"
#include <cassert>
#include <WinSock2.h>

bool Wsa::isWsaStartedUp = false;

Wsa::Wsa()
{
    assert(!isWsaStartedUp);
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    assert(result == 0);
    isWsaStartedUp = true;
}

Wsa::~Wsa()
{
    WSACleanup();
}