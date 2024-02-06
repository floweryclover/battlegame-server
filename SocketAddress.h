//
// Created by floweryclover on 2024-02-06.
//

#ifndef BATTLEGAME_SERVER_SOCKETADDRESS_H
#define BATTLEGAME_SERVER_SOCKETADDRESS_H

#include <memory>
#include <string>

class SocketAddress {
public:
    explicit SocketAddress() noexcept;
    explicit SocketAddress(const char* hostIpAddress, unsigned short hostPort) noexcept;
    explicit SocketAddress(const struct sockaddr_in& rawSockAddrIn) noexcept;
    SocketAddress(const SocketAddress& rhs) noexcept;
    SocketAddress(SocketAddress&& rhs) noexcept;
    SocketAddress& operator=(const SocketAddress& rhs) noexcept;
    SocketAddress& operator=(SocketAddress&& rhs) noexcept;
    ~SocketAddress();

    std::string ToString() const;
    const struct sockaddr_in& GetRawSockAddrIn() const;

private:
    std::unique_ptr<struct sockaddr_in> mpRawSockAddrIn;
};


#endif //BATTLEGAME_SERVER_SOCKETADDRESS_H
