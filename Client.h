//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_CLIENT_H
#define BATTLEGAME_SERVER_CLIENT_H

#include <array>
#include <expected>
#include <optional>
#include <memory>
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
    explicit Client(ClientId clientId, Socket&& socket) noexcept;
    Client(Client&& rhs) noexcept;
    ~Client() = default;

    inline ClientId GetClientId() const { return this->mClientId; }
    inline const Socket& GetTcpSocket() const { return this->mTcpSocket; }

    void Tick();
private:
    std::expected<std::optional<std::unique_ptr<class Message>>, std::optional<ErrorCode>> ReceiveTcp();
    const ClientId mClientId;
    const Socket mTcpSocket;
    int mCurrentReceived;
    int mTotalSizeToReceive;
    int mLastReceivedHeaderType;
    bool mIsReceivingHeader;
    std::array<char, MAX_MESSAGE_SIZE> mReceiveBuffer;
};


#endif //BATTLEGAME_SERVER_CLIENT_H
