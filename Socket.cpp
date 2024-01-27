//
// Created by floweryclover on 2024-01-25.
//

#include "Socket.h"
#include <cassert>
#ifdef _WIN32
#include <WinSock2.h>
#elifdef linux
#include <sys/socket.h>
#endif

Socket::Socket(int hSocket) : hSocket_(hSocket)
{
    assert(hSocket_ != -1);
}

Socket::~Socket()
{

#ifdef _WIN32
    shutdown(hSocket_, SD_SEND);
    closesocket(hSocket_);
#elifdef linux
    shutdown(hSocket_, SHUT_WR);
#endif
}