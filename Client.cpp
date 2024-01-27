//
// Created by floweryclover on 2024-01-27.
//

#include "Client.h"
#include <utility>
#include <cstring>

Client::Client(int connectionId, Socket&& socket) noexcept : connectionId_(connectionId), socket_(std::move(socket))
{
    this->currentReceived = 0;
    this->totalSizeToReceive = HEADER_SIZE;
    this->receivingHeaderType = 0;
    memset(this->receiveBuffer, 0, MAX_MESSAGE_SIZE);
}

Client::Client(Client&& rhs) noexcept : Client(rhs.connectionId_, std::move(rhs.socket_))
{
}

Client::~Client() = default;
