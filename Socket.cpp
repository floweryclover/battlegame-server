//
// Created by floweryclover on 2024-01-25.
//

#include "Socket.h"
#include <cassert>
#ifdef _WIN32
#include <WinSock2.h>
#elifdef linux
#include <sys/socket.h>
#define INVALID_SOCKET -1
#endif

Socket::Socket() noexcept : hSocket_(INVALID_SOCKET) {}

Socket::Socket(SOCKET hSocket) noexcept : hSocket_(hSocket) {}


Socket::Socket(Socket &&rhs) noexcept
{
    this->hSocket_ = rhs.hSocket_;
    rhs.hSocket_ = INVALID_SOCKET;
}

Socket::~Socket()
{
    if (hSocket_ != INVALID_SOCKET)
    {
#ifdef _WIN32
        shutdown(hSocket_, SD_SEND);
        closesocket(hSocket_);
#elifdef linux
        shutdown(hSocket_, SHUT_WR);
        close(hSocket_);
#endif
        hSocket_ = INVALID_SOCKET;
    }
}


