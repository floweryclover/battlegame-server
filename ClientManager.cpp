//
// Created by floweryclover on 2024-01-30.
//

#include <iostream>
#include <cassert>
#include "ClientManager.h"
#include "BattleGameServer.h"
#include "GameData.h"
#include "StcRpc.h"
#include "Constants.h"
#include "Context.h"
#include "Socket.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#elifdef linux
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <cerrno>
#endif

ClientManager::ClientManager(const char* listenIpAddress, unsigned short listenPort) : mCurrentSent(0), mNewClientNumber(0)
{
    StcRpc::RegisterSendEnqueueFunction(
            [this](ConnectionId id, Message&& message)
            {
                mSendQueue.emplace(id, std::move(message));
            }
            );
    mpListenSocket = std::make_unique<Socket>(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    int option = 1;
    int result = setsockopt(mpListenSocket->AsHandle(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    assert(result != -1);

    struct sockaddr_in sockAddrIn {};
    memset(&sockAddrIn, 0, sizeof(struct sockaddr_in));
    result = inet_pton(AF_INET, listenIpAddress, &sockAddrIn.sin_addr);
    assert(result == 1);
    sockAddrIn.sin_family = AF_INET;
    sockAddrIn.sin_port = htons(listenPort);
    result = bind(mpListenSocket->AsHandle(), (struct sockaddr*)&sockAddrIn, sizeof(sockAddrIn));
    assert(result != -1);

    result = listen(mpListenSocket->AsHandle(), 8);
    assert(result != -1);

#ifdef _WIN32
    u_long nonblockingFlag = 1;
    result = ioctlsocket(mpListenSocket->AsHandle(), FIONBIO, &nonblockingFlag);
#elifdef linux
    unsigned long nonblockingFlag = 1;
    result = ioctl(mpListenSocket->AsHandle(), FIONBIO, &nonblockingFlag);
#endif
    assert(result != -1);
}

void ClientManager::ProcessNetworkIoOnce()
{
    Socket clientSocket(accept(mpListenSocket->AsHandle(), nullptr, nullptr));
    if (clientSocket.AsHandle() == INVALID_SOCKET)
    {
#ifdef _WIN32
        int errorCode = WSAGetLastError();
        assert(errorCode == WSAEWOULDBLOCK);
#elifdef linux
        int errorCode = errno;
        assert(errorCode == EWOULDBLOCK);
#endif
    }
    else
    {
#ifdef _WIN32
        u_long nonblockingFlag = 1;
        int result = ioctlsocket(socket, FIONBIO, &nonblockingFlag);
#elifdef linux
        unsigned long nonblockingFlag = 1;
        int result = ioctl(clientSocket.AsHandle(), FIONBIO, &nonblockingFlag);
#endif
        assert(result != -1);
        std::cout << "[접속] 클라이언트 " << mNewClientNumber << std::endl;
        BattleGameServer::GetInstance().GetGameData().OnPlayerConnected(mNewClientNumber);
        mClients.emplace(mNewClientNumber, Client(mNewClientNumber, std::move(clientSocket)));
        mNewClientNumber++;
    }

    if (!mSendQueue.empty())
    {
        auto& messagePair = mSendQueue.front();
        if (!IsClientExists(messagePair.first))
        {
            mSendQueue.pop();
        }
        else
        {
            const Client& targetClient = mClients.at(messagePair.first);
            const Message& message = messagePair.second;

            const char* pSendBuffer = (mCurrentSent < HEADER_SIZE ? reinterpret_cast<const char*>(&message) : message.mBodyBuffer) + mCurrentSent;
            int lengthToSend = (mCurrentSent < HEADER_SIZE ? HEADER_SIZE : message.mHeaderBodySize) - mCurrentSent;
            int result = send(targetClient.GetSocket().AsHandle(), pSendBuffer, lengthToSend, 0);
            if (result < 0)
            {
#ifdef _WIN32
                int errorCode = WSAGetLastError();
                if (errorCode != WSAEWOULDBLOCK)
#elifdef linux
                int errorCode = errno;
                if (errorCode != EWOULDBLOCK)
#endif
                {
                    std::cerr << "[에러] 클라이언트 " << targetClient.GetConnectionId() << "에게 데이터를 전송하던 도중 에러가 발생하였습니다: 에러 코드" << errorCode << "." << std::endl;
                }
            }
            else if (result == 0)
            {
                std::cout << "[접속 해제] 클라이언트 " << targetClient.GetConnectionId() << std::endl;
                BattleGameServer::GetInstance().GetGameData().OnPlayerDisconnected(targetClient.GetConnectionId());
                mCurrentSent = 0;
                mClients.erase(targetClient.GetConnectionId());
                mSendQueue.pop();
            }
            else
            {
                mCurrentSent += result;
                if (mCurrentSent == HEADER_SIZE + message.mHeaderBodySize)
                {
                    mCurrentSent = 0;
                    mSendQueue.pop();
                }
            }
        }
    }

    for (auto iter = mClients.begin(); iter != mClients.end();)
    {
        Client& client = iter->second;

        auto receiveResult = client.Receive();
        if (receiveResult.has_value())
        {
            if (receiveResult.value().has_value())
            {
                Context context(client.GetConnectionId());
                mCtsRpc.HandleMessage(context, *receiveResult.value().value());
            }
        }
        else
        {
            if (receiveResult.error().has_value())
            {
                std::cerr << "[에러] 클라이언트 " << client.GetConnectionId() << " 에게서 데이터를 수신하던 도중 에러가 발생하였습니다: 에러 코드 " << receiveResult.error().value() << "." << std::endl;
            }
            std::cout << "[접속 해제] 클라이언트 " << client.GetConnectionId() << std::endl;
            BattleGameServer::GetInstance().GetGameData().OnPlayerDisconnected(client.GetConnectionId());
            mClients.erase(iter++);
            continue;
        }
        iter++;
    }
}