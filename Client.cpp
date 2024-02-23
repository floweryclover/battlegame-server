//
// Created by floweryclover on 2024-01-27.
//

#include "Client.h"
#include "Message.h"
#include "Context.h"
#include "CtsRpc.h"
#include "BattleGameServer.h"
#include "ClientManager.h"
#include <iostream>
#include <utility>
#include <memory>
#include <cstring>

#ifdef _WIN32
#include <WinSock2.h>
#elifdef linux
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cerrno>
#endif

Client::Client(ClientId clientId, Socket&& tcpSocket, SocketAddress&& tcpSocketAddress) noexcept
: mClientId(clientId),
mTcpSocket(std::move(tcpSocket)),
mIsReceivingHeader(true),
mCurrentReceived(0),
mTotalSizeToReceive(HEADER_SIZE),
mLastReceivedHeaderType(0),
mTcpSocketAddress(std::move(tcpSocketAddress))
{}

Client::Client(Client&& rhs) noexcept : Client(rhs.mClientId, const_cast<Socket&&>(std::move(rhs.mTcpSocket)), std::move(rhs.mTcpSocketAddress)) {}

Client::~Client() {}

void Client::Tick()
{
    auto receiveResult = this->ReceiveTcp();
    if (receiveResult.has_value())
    {
        if (receiveResult.value().has_value())
        {
            Context context(mClientId, SendReliability::RELIABLE);
            BattleGameServer::GetConstInstance()
            .GetConstCtsRpc()
            .HandleMessage(context, *receiveResult.value().value());
        }
    }
    else
    {
        if (receiveResult.error().has_value())
        {
            std::cerr << "[에러] 클라이언트 " << mClientId << " 에게서 데이터를 수신하던 도중 에러가 발생하였습니다: 에러 코드 " << receiveResult.error().value() << "." << std::endl;
        }
        BattleGameServer::GetInstance()
        .GetClientManager()
        .InvokeOnPlayerDisconnected(mClientId);
    }
}

std::expected<std::optional<std::unique_ptr<Message>>, std::optional<ErrorCode>> Client::ReceiveTcp() {
    int sizeToReceive = mTotalSizeToReceive - mCurrentReceived;
    int result = recv(GetTcpSocket().GetRawHandle(), mReceiveBuffer.data() + mCurrentReceived, sizeToReceive, 0);
    if (result == -1)
    {
#ifdef _WIN32
        int errorCode = WSAGetLastError();
        if (errorCode == WSAEWOULDBLOCK)
#elifdef linux
        int errorCode = errno;
        if (errorCode == EWOULDBLOCK)
#endif
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
            if (mIsReceivingHeader)
            {
                memcpy(&mTotalSizeToReceive, mReceiveBuffer.data(), 4);
                memcpy(&mLastReceivedHeaderType, mReceiveBuffer.data() + 4, 4);

                if (mTotalSizeToReceive == 0) // 바디 없는 메시지
                {
                    std::unique_ptr<Message> pMessage(new Message(0, mLastReceivedHeaderType, nullptr));
                    mTotalSizeToReceive = HEADER_SIZE;
                    mIsReceivingHeader = true;
                    return pMessage;
                }
                else
                {
                    mIsReceivingHeader = false;
                    return std::nullopt;
                }
            }
            else
            {
                std::unique_ptr<Message> pMessage(new Message(mTotalSizeToReceive, mLastReceivedHeaderType, mReceiveBuffer.data()));
                mTotalSizeToReceive = HEADER_SIZE;
                mIsReceivingHeader = true;
                return pMessage;
            }
        }
        else
        {
            return std::nullopt;
        }
    }
}
