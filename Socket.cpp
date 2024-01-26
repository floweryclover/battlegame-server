//
// Created by floweryclover on 2024-01-25.
//

#include "Socket.h"
#include <cassert>
#ifdef _WIN32
#include <WinSock2.h>
#endif

Socket::Socket(int hSocket) : hSocket_(hSocket)
{
    assert(hSocket_ != -1);
}

Socket::~Socket()
{
    shutdown(hSocket_, SD_SEND);
#ifdef _WIN32
    closesocket(hSocket_);
#endif
}