//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_CLIENT_H
#define BATTLEGAME_SERVER_CLIENT_H

#include <array>
#include <expected>
#include <optional>
#include <memory>
#include <string>
#include "Socket.h"
#include "Constants.h"
#include "IoResult.h"

using ErrorCode = int;
using SerializedEndpoint = unsigned long long;
using ClientId = SerializedEndpoint;

class Client {
public:
    Client(const Client& rhs) = delete;
    Client& operator=(const Client& rhs) = delete;

    explicit Client(ClientId clientId, Socket&& tcpSocket, const struct sockaddr_in* pSockaddrIn, int addrLen) noexcept;
    Client(Client&& rhs) noexcept;
    ~Client();

    inline ClientId GetClientId() const { return this->mClientId; }
    inline Socket& GetTcpSocket() { return this->mTcpSocket; }
    inline const struct sockaddr_in* GetTcpSockAddrIn() const { return this->mpTcpSockaddrIn.get(); }
    inline int GetTcpSockAddrLen() { return this->mAddrLen; }
    inline const std::string& GetEndpointString() const { return this->mEndpoint; }

    void Tick();
private:
    std::string mEndpoint;
    std::unique_ptr<struct sockaddr_in> mpTcpSockaddrIn;
    std::unique_ptr<struct sockaddr_in> mpUdpSockaddrIn;
    int mAddrLen;
    std::expected<std::optional<std::unique_ptr<class Message>>, std::optional<ErrorCode>> ReceiveTcp();
    ClientId mClientId;
    Socket mTcpSocket;
    int mCurrentReceived;
    int mTotalSizeToReceive;
    int mLastReceivedHeaderType;
    bool mIsReceivingHeader;
    std::array<char, MAX_MESSAGE_SIZE> mReceiveBuffer;
};


#endif //BATTLEGAME_SERVER_CLIENT_H
