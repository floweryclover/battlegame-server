//
// Created by floweryclover on 2024-01-30.
//

#include <iostream>
#include <cassert>
#include "ClientManager.h"
#include "BattleGameServer.h"
#include "GameData.h"
#include "StcRpc.h"
#include "CtsRpc.h"
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

ClientManager::ClientManager(const char* listenIpAddress, unsigned short listenPort) : mCurrentSent(0)
{
    mpListenSocket = std::make_unique<Socket>(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    mpUdpSocket = std::make_unique<Socket>(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));

    int option = 1;
    int result = setsockopt(mpListenSocket->AsHandle(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    assert(result != -1);
    result = setsockopt(mpUdpSocket->AsHandle(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    assert(result != -1);

    struct sockaddr_in sockAddrIn {};
    memset(&sockAddrIn, 0, sizeof(struct sockaddr_in));
    result = inet_pton(AF_INET, listenIpAddress, &sockAddrIn.sin_addr);
    assert(result == 1);
    sockAddrIn.sin_family = AF_INET;
    sockAddrIn.sin_port = htons(listenPort);
    result = bind(mpListenSocket->AsHandle(), (struct sockaddr*)&sockAddrIn, sizeof(sockAddrIn));
    assert(result != -1);
    result = bind(mpUdpSocket->AsHandle(), (struct sockaddr*)&sockAddrIn, sizeof(sockAddrIn));
    assert(result != -1);

    result = listen(mpListenSocket->AsHandle(), 8);
    assert(result != -1);

#ifdef _WIN32
    u_long nonblockingFlag = 1;
    result = ioctlsocket(mpListenSocket->AsHandle(), FIONBIO, &nonblockingFlag);
    assert(result != -1);
    result = ioctlsocket(mpUdpSocket->AsHandle(), FIONBIO, &nonblockingFlag);
    assert(result != -1);
#elifdef linux
    unsigned long nonblockingFlag = 1;
    result = ioctl(mpListenSocket->AsHandle(), FIONBIO, &nonblockingFlag);
    assert(result != -1);
    result = ioctl(mpUdpSocket->AsHandle(), FIONBIO, &nonblockingFlag);
    assert(result != -1);
#endif

}

void ClientManager::Tick()
{
    struct sockaddr_in clientAddr {};
    int clientAddrLen = sizeof(clientAddr);
    Socket clientSocket(accept(mpListenSocket->AsHandle(), reinterpret_cast<struct sockaddr*>(&clientAddr), reinterpret_cast<socklen_t*>(&clientAddrLen)));
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
        ClientId serialized = (ntohl(clientAddr.sin_addr.s_addr) << 16) + (ntohs(clientAddr.sin_port));
        mClients.emplace(serialized, Client(serialized, std::move(clientSocket), &clientAddr, clientAddrLen));
        this->InvokeOnPlayerConnected(serialized);
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
            int result = send(targetClient.GetTcpSocket().AsHandle(), pSendBuffer, lengthToSend, 0);
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
                    std::cerr << "[에러] 클라이언트 " << targetClient.GetClientId() << "에게 데이터를 전송하던 도중 에러가 발생하였습니다: 에러 코드" << errorCode << "." << std::endl;
                }
            }
            else if (result == 0)
            {
                std::cout << "[접속 해제] 클라이언트 " << targetClient.GetClientId() << std::endl;
                InvokeOnPlayerDisconnected(targetClient.GetClientId());
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
        if (!mClients.contains(iter->first))
        {
            continue;
        }
        Client& client = iter->second;
        client.Tick();
        iter++;
    }

    struct sockaddr_in udpReceiveAddr {};
    int udpReceiveAddrSize = sizeof(udpReceiveAddr);
    int result = recvfrom(mpUdpSocket->AsHandle(), mUdpReceiveBuffer.data(), MAX_MESSAGE_SIZE, 0, reinterpret_cast<struct sockaddr*>(&udpReceiveAddr), reinterpret_cast<socklen_t*>(&udpReceiveAddrSize));
    if (result < 0)
    {
        int errorCode = errno;
        assert(errorCode == EWOULDBLOCK);
    }
    else
    {
        assert(result > 0);

        SerializedEndpoint serialized = (ntohl(udpReceiveAddr.sin_addr.s_addr) << 16) + (ntohs(udpReceiveAddr.sin_port));
        if (!mClients.contains(serialized))
        {
            std::cerr << "정상적으로 접속처리되지 않은 클라이언트로부터 UDP 패킷을 수신했습니다: " << inet_ntoa(udpReceiveAddr.sin_addr) << ":" << ntohs(udpReceiveAddr.sin_port) << std::endl;
        }
        else
        {
            Message message;
            memcpy(&message.mHeaderBodySize, mUdpReceiveBuffer.data(), 4);
            memcpy(&message.mHeaderMessageType, mUdpReceiveBuffer.data()+4, 4);
            char* body = new char[message.mHeaderBodySize];
            memcpy(body, mUdpReceiveBuffer.data()+8, message.mHeaderBodySize);
            message.mBodyBuffer = body;
            BattleGameServer::GetConstInstance()
            .GetConstCtsRpc()
            .HandleMessage(Context(serialized), message);
        }
    }
}

void ClientManager::InvokeOnPlayerDisconnected(ClientId clientId)
{
    std::cout << "[접속 해제] 클라이언트 " << clientId << " (" << mClients.at(clientId).GetEndpointString() << ")" << std::endl;
    BattleGameServer::GetInstance().GetGameData().OnPlayerDisconnected(clientId);
    mClients.erase(clientId);
    BattleGameServer::GetInstance()
    .GetGameData()
    .OnPlayerDisconnected(clientId);
}

void ClientManager::InvokeOnPlayerConnected(ClientId clientId)
{
    std::cout << "[접속] 클라이언트 " << clientId << std::endl;
    BattleGameServer::GetInstance().GetGameData().OnPlayerConnected(clientId);
}