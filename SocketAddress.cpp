//
// Created by floweryclover on 2024-02-06.
//

#include "SocketAddress.h"
#include <cassert>

#ifdef _WIN32
#include <ws2tcpip.h>
#elifdef linux
#include <arpa/inet.h>
#endif

SocketAddress::SocketAddress() noexcept : mpRawSockAddrIn(nullptr) {}

SocketAddress::SocketAddress(const char *hostIpAddress, unsigned short hostPort) noexcept
{
#ifdef _WIN32
    in_addr inAddr;
    ZeroMemory(&inAddr, sizeof(in_addr));
#elifdef linux
    in_addr_t inAddr;
#endif
    assert (inet_pton(AF_INET, hostIpAddress, &inAddr) != -1);
    auto pRawSockAddrIn = new struct sockaddr_in;
    memset(pRawSockAddrIn, 0, sizeof(struct sockaddr_in));
    pRawSockAddrIn->sin_family = AF_INET;
    pRawSockAddrIn->sin_port = htons(hostPort);
    pRawSockAddrIn->sin_addr = inAddr;
    mpRawSockAddrIn = std::unique_ptr<struct sockaddr_in>(pRawSockAddrIn);
}

SocketAddress::SocketAddress(const struct sockaddr_in &rawSockAddrIn) noexcept
{
    auto pRawSockAddrIn = new struct sockaddr_in;
    memcpy(pRawSockAddrIn, &rawSockAddrIn, sizeof(struct sockaddr_in));
    mpRawSockAddrIn = std::unique_ptr<struct sockaddr_in>(pRawSockAddrIn);
}

SocketAddress::SocketAddress(const SocketAddress &rhs) noexcept : SocketAddress(*rhs.mpRawSockAddrIn) {}

SocketAddress::SocketAddress(SocketAddress &&rhs) noexcept : mpRawSockAddrIn(std::move(rhs.mpRawSockAddrIn)) {}

SocketAddress::~SocketAddress() = default;

SocketAddress& SocketAddress::operator=(const SocketAddress &rhs) noexcept
{
    this->mpRawSockAddrIn->sin_addr = rhs.mpRawSockAddrIn->sin_addr;
    this->mpRawSockAddrIn->sin_port = rhs.mpRawSockAddrIn->sin_port;
    return *this;
}

SocketAddress& SocketAddress::operator=(SocketAddress &&rhs) noexcept
{
    this->mpRawSockAddrIn.reset();
    this->mpRawSockAddrIn = std::move(rhs.mpRawSockAddrIn);
    return *this;
}

std::string SocketAddress::ToString() const
{
    if (mpRawSockAddrIn == nullptr)
    {
        return {};
    }

    return {inet_ntoa(mpRawSockAddrIn->sin_addr)};
}

const struct sockaddr_in& SocketAddress::GetRawSockAddrIn() const
{
    assert(mpRawSockAddrIn != nullptr);
    return *mpRawSockAddrIn;
}