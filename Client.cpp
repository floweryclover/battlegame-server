//
// Created by floweryclover on 2024-01-27.
//

#include "Client.h"
#include "Message.h"
#include <utility>
#include <memory>
#include <cstring>

#ifdef _WIN32
#include <WinSock2.h>
#elifdef linux
#include <sys/socket.h>
#include <cerrno>
#endif

Client::Client(int connectionId, Socket&& socket) noexcept : mConnectionId(connectionId), mSocket(std::move(socket))
{
    this->mCurrentReceived = 0;
    this->mTotalSizeToReceive = HEADER_SIZE;
    this->mLastReceivedHeaderType = 0;
}

Client::Client(Client&& rhs) noexcept : Client(rhs.mConnectionId, const_cast<Socket&&>(std::move(rhs.mSocket)))
{
}

std::expected<std::optional<std::unique_ptr<Message>>, std::optional<ErrorCode>> Client::Receive() {
    int sizeToReceive = mTotalSizeToReceive - mCurrentReceived;
    int result = recv(GetSocket().AsHandle(), mReceiveBuffer.data() + mCurrentReceived, sizeToReceive, 0);
    if (result == -1)
    {
        int errorCode = errno;
        if (errorCode == EWOULDBLOCK)
        {
            return std::nullopt;
        }
        else
        {
            return std::unexpected(errorCode);
        }
    }
    else if (result == 0)
    {
        return std::unexpected(std::nullopt);
    }
    else
    {
        mCurrentReceived += result;
        if (mCurrentReceived == mTotalSizeToReceive)
        {
            mCurrentReceived = 0;
            if (mTotalSizeToReceive == HEADER_SIZE)
            {
                memcpy(&mTotalSizeToReceive, mReceiveBuffer.data(), 4);
                memcpy(&mLastReceivedHeaderType, mReceiveBuffer.data() + 4, 4);
                return std::nullopt;
            }
            else
            {
                std::unique_ptr<Message> pMessage(new Message(mTotalSizeToReceive, mLastReceivedHeaderType, mReceiveBuffer.data()));
                mTotalSizeToReceive = HEADER_SIZE;
                return pMessage;
            }
        }
        else
        {
            return std::nullopt;
        }
    }
}
