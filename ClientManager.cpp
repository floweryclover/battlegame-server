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
#include "SocketAddress.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#elifdef linux
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <cerrno>
#endif

ClientManager::ClientManager(const char* listenIpAddress, unsigned short listenPort) : mCurrentSent(0), mUdpSendBuffer(), mUdpReceiveBuffer()
{
    mpListenSocket = std::make_unique<Socket>(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    mpUdpSocket = std::make_unique<Socket>(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));

    int option = 1;
    int result = setsockopt(mpListenSocket->GetRawHandle(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    assert(result != -1);
    result = setsockopt(mpUdpSocket->GetRawHandle(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    assert(result != -1);

    SocketAddress listenSocketAddress(listenIpAddress, listenPort);
    result = bind(mpListenSocket->GetRawHandle(), reinterpret_cast<const struct sockaddr*>(&listenSocketAddress.GetRawSockAddrIn()), sizeof(struct sockaddr_in));
    assert(result != -1);
    result = bind(mpUdpSocket->GetRawHandle(), reinterpret_cast<const struct sockaddr*>(&listenSocketAddress.GetRawSockAddrIn()), sizeof(struct sockaddr_in));
    assert(result != -1);

    result = listen(mpListenSocket->GetRawHandle(), 8);
    assert(result != -1);

#ifdef _WIN32
    u_long nonblockingFlag = 1;
    result = ioctlsocket(mpListenSocket->AsHandle(), FIONBIO, &nonblockingFlag);
    assert(result != -1);
    result = ioctlsocket(mpUdpSocket->AsHandle(), FIONBIO, &nonblockingFlag);
    assert(result != -1);
#elifdef linux
    unsigned long nonblockingFlag = 1;
    result = ioctl(mpListenSocket->GetRawHandle(), FIONBIO, &nonblockingFlag);
    assert(result != -1);
    result = ioctl(mpUdpSocket->GetRawHandle(), FIONBIO, &nonblockingFlag);
    assert(result != -1);
#endif

}

void ClientManager::Tick()
{
    struct sockaddr_in clientAddr {};
    int clientAddrLen = sizeof(clientAddr);
    Socket clientSocket(accept(mpListenSocket->GetRawHandle(), reinterpret_cast<struct sockaddr*>(&clientAddr), reinterpret_cast<socklen_t*>(&clientAddrLen)));
    if (clientSocket.GetRawHandle() == INVALID_SOCKET)
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
        int result = ioctl(clientSocket.GetRawHandle(), FIONBIO, &nonblockingFlag);
#endif
        assert(result != -1);
        ClientId serialized = (ntohl(clientAddr.sin_addr.s_addr) << 16) + (ntohs(clientAddr.sin_port));
        mClients.emplace(serialized, Client(serialized, std::move(clientSocket), SocketAddress(clientAddr)));
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
            Client& targetClient = mClients.at(messagePair.first);
            const Message& message = messagePair.second;

            const char* pSendBuffer = (mCurrentSent < HEADER_SIZE ? reinterpret_cast<const char*>(&message) : message.mpBodyBuffer.get() - HEADER_SIZE) + mCurrentSent;
            int lengthToSend = (mCurrentSent < HEADER_SIZE ? HEADER_SIZE : message.mHeaderBodySize + HEADER_SIZE) - mCurrentSent;
            int result = send(targetClient.GetTcpSocket().GetRawHandle(), pSendBuffer, lengthToSend, 0);
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
            iter++;
            continue;
        }
        Client& client = (iter++)->second;
        client.Tick();
    }

    struct sockaddr_in udpReceiveAddr {};
    int udpReceiveAddrSize = sizeof(udpReceiveAddr);
    int result = recvfrom(mpUdpSocket->GetRawHandle(), mUdpReceiveBuffer.data(), MAX_MESSAGE_SIZE, 0, reinterpret_cast<struct sockaddr*>(&udpReceiveAddr), reinterpret_cast<socklen_t*>(&udpReceiveAddrSize));
    if (result < 0)
    {
        int errorCode = errno;
        assert(errorCode == EWOULDBLOCK);
    }
    else
    {
        assert(result > 0);

        SerializedEndpoint serialized = (ntohl(udpReceiveAddr.sin_addr.s_addr) << 16) + (ntohs(udpReceiveAddr.sin_port));
        Message message;
        memcpy(&message.mHeaderBodySize, mUdpReceiveBuffer.data(), 4);
        memcpy(&message.mHeaderMessageType, mUdpReceiveBuffer.data()+4, 4);
        char* body = new char[message.mHeaderBodySize];
        memcpy(body, mUdpReceiveBuffer.data()+8, message.mHeaderBodySize);
        message.mpBodyBuffer = std::unique_ptr<char>(body);
        if (!mUdpTcpMap.contains(serialized))
        {
            // 첫 UDP 메시지의 경우 무조건 토큰을 담고 있음
            // 그렇지 않은 경우 유효하지 않은 UDP 연결이라고 간주
            if (message.mHeaderMessageType != CtsRpc::CTS_ACK_UDP_TOKEN)
            {
                std::cerr << "클라이언트 UDP:" << serialized << " 가 유효하지 않은 UDP 인증 토큰을 보냈거나 인증되지 않은 연결입니다." << std::endl;
            }
            else
            {
                ClientId token;
                memcpy(&token, message.mpBodyBuffer.get(), 8);
                if (mTcpUdpMap.contains(token))
                {
                    std::cerr << "클라이언트 UDP:" << serialized << " 가 UDP 인증 토큰을 보냈으나 해당 TCP 소켓과 일치하는 UDP 소켓이 이미 등록되어 있습니다." << std::endl;
                }
                else
                {
                    mUdpTcpMap.emplace(serialized, token);
                    mTcpUdpMap.emplace(token, serialized);
                    mUdpSocketAddresses.emplace(serialized, SocketAddress(udpReceiveAddr));
                }
            }
        }
        else
        {
            // TCP 소켓 ID로 변환해서 호출
            BattleGameServer::GetConstInstance()
            .GetConstCtsRpc()
            .HandleMessage(Context(mUdpTcpMap.at(serialized), SendReliability::UNRELIABLE), message);
        }
    }
}

void ClientManager::RequestSendMessage(MessageReliability reliability, ClientId targetClientId, Message&& message)
{
    if (!mClients.contains(targetClientId))
    {
        return;
    }
    if (reliability == MessageReliability::RELIABLE)
    {
        mSendQueue.emplace(targetClientId, std::move(message));
    }
    else
    {
        if (!mTcpUdpMap.contains(targetClientId))
        {
            return;
        }
        const auto& udpSocketAddress = mUdpSocketAddresses.at(mTcpUdpMap.at(targetClientId));
        memcpy(mUdpSendBuffer.data(), &message.mHeaderBodySize, 4);
        memcpy(mUdpSendBuffer.data()+4, &message.mHeaderMessageType, 4);
        memcpy(mUdpSendBuffer.data()+8, message.mpBodyBuffer.get(), message.mHeaderBodySize);
        int result = sendto(mpUdpSocket->GetRawHandle(), mUdpSendBuffer.data(), 4 + 4 + message.mHeaderBodySize, 0, reinterpret_cast<const struct sockaddr*>(&udpSocketAddress.GetRawSockAddrIn()), sizeof(struct sockaddr_in));
        if (result < 0)
        {
            int errorCode = errno;
            if (errno == EWOULDBLOCK)
            {
                std::cerr << "[전송 에러] 클라이언트 " << targetClientId << " 에게 비신뢰성 메시지를 전송하려 했으나 Would block 상태입니다." << std::endl;
            }
            else
            {
                std::cerr << "[전송 에러] 클라이언트 " << targetClientId << " 에게 비신뢰성 메시지를 전송하던 도중 에러가 발생하였습니다: " << errorCode << std::endl;
            }

        }
    }
}

bool ClientManager::KickPlayer(ClientId clientId)
{
    if (!mClients.contains(clientId))
    {
        return false;
    }

    mClients.at(clientId).GetTcpSocket().Shutdown();
    return true;
}

void ClientManager::InvokeOnPlayerDisconnected(ClientId clientId)
{
    std::cout << "[접속 해제] 클라이언트 " << clientId << " (" << mClients.at(clientId).GetTcpSocketAddress().ToString() << ")" << std::endl;

    if (mTcpUdpMap.contains(clientId))
    {
        mUdpTcpMap.erase(mTcpUdpMap.at(clientId));
        mTcpUdpMap.erase(clientId);
    }

    BattleGameServer::GetInstance()
    .GetGameData()
    .OnPlayerDisconnected(clientId);
    mClients.erase(clientId);
}

void ClientManager::InvokeOnPlayerConnected(ClientId clientId)
{
    // mClient에 유저를 넣는 것은 accept()하는 부분에서 진행
    std::cout << "[접속] 클라이언트 " << clientId <<  " (" << mClients.at(clientId).GetTcpSocketAddress().ToString() << ")" << std::endl;

    BattleGameServer::GetInstance()
    .GetGameData()
    .OnPlayerConnected(clientId);

    BattleGameServer::GetConstInstance()
    .GetConstStcRpc()
    .AssignUdpToken(clientId);
}