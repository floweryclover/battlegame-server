//
// Created by floweryclover on 2024-01-27.
//

#include "Client.h"
#include <utility>
#include <cstring>

Client::Client(int connectionId, Socket&& socket) noexcept : mConnectionId(connectionId), mSocket(std::move(socket))
{
    this->mCurrentReceived = 0;
    this->mTotalSizeToReceive = HEADER_SIZE;
    this->mLastReceivedHeaderType = 0;
    memset(this->mReceiveBuffer, 0, MAX_MESSAGE_SIZE);
}

Client::Client(Client&& rhs) noexcept : Client(rhs.mConnectionId, std::move(rhs.mSocket))
{
}

Client::~Client() = default;
