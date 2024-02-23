//
// Created by floweryclover on 2024-01-25.
//

#include "Socket.h"
#include "Constants.h"
#include <cassert>
#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#elifdef linux
#include <unistd.h>
#include <sys/socket.h>
#endif

Socket::Socket() noexcept : mSocketHandle(INVALID_SOCKET) {}

Socket::Socket(SOCKET hSocket) noexcept : mSocketHandle(hSocket) {}


Socket::Socket(Socket &&rhs) noexcept
{
    this->mSocketHandle = rhs.mSocketHandle;
    rhs.mSocketHandle = INVALID_SOCKET;
}

Socket::~Socket()
{
    Shutdown();
}

void Socket::Shutdown()
{
    if (mSocketHandle != INVALID_SOCKET)
    {
#ifdef _WIN32
        shutdown(mSocketHandle, SD_SEND);
        closesocket(mSocketHandle);
#elifdef linux
        shutdown(mSocketHandle, SHUT_WR);
        close(mSocketHandle);
#endif
        mSocketHandle = INVALID_SOCKET;
    }
}

