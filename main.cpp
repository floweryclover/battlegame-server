#include <iostream>
#include <map>
#include <queue>
#include <memory>
#include <algorithm>
#include <cassert>
#include <cstring>

#include "Message.h"
#include "Socket.h"
#include "Client.h"
#include "Constants.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "Wsa.h"
#pragma comment(lib, "ws2_32.lib")
#elifdef linux
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <cerrno>
#endif

enum class IOResult
{
    WOULD_BLOCK,
    DISCONNECTED,
    FATAL_ERROR,
    SUCCESSFUL,
};

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <listen-address> <listen-port>" << std::endl;
        return 1;
    }

    auto setNonblocking = [](SOCKET socket)
            {
#ifdef _WIN32
                u_long nonblockingFlag = 1;
                return ioctlsocket(socket, FIONBIO, &nonblockingFlag);
#elifdef linux
                unsigned long nonblockingFlag = 1;
                return ioctl(socket, FIONBIO, &nonblockingFlag);
#endif
            };

#ifdef _WIN32
    Wsa wsa;
    SetConsoleOutputCP(CP_UTF8);
#endif
    Socket listenSocket(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));

    struct sockaddr_in sockAddrIn {};
    memset(&sockAddrIn, 0, sizeof(struct sockaddr_in));
    int result = inet_pton(AF_INET, argv[1], &sockAddrIn.sin_addr);
    assert(result == 1);
    sockAddrIn.sin_family = AF_INET;
    sockAddrIn.sin_port = htons(atoi(argv[2]));
    result = bind(listenSocket, (struct sockaddr*)&sockAddrIn, sizeof(sockAddrIn));
    assert(result != -1);

    result = listen(listenSocket, 8);
    assert(result != -1);

    result = setNonblocking(listenSocket);
    assert (result != -1);

    auto handleResult = [](int ioReturnValue, int& outErrorCode)
            {
                if (ioReturnValue < 0)
                {
#ifdef _WIN32
                    outErrorCode = WSAGetLastError();
                    if (outErrorCode == WSAEWOULDBLOCK)
#elifdef linux
                    outErrorCode = errno;
                    if (outErrorCode == EWOULDBLOCK)
#endif
                    {
                        return IOResult::WOULD_BLOCK;
                    }
                    else
                    {
                        return IOResult::FATAL_ERROR;
                    }
                }
                else if (ioReturnValue == 0)
                {
                    return IOResult::DISCONNECTED;
                }
                else
                {
                    return IOResult::SUCCESSFUL;
                }
            };

    int errorCode;
    std::queue<Message> sendQueue;
    std::map<unsigned int, std::unique_ptr<Client>> clients;
    unsigned int clientNumber = 0;
    int currentSent = 0;
    while (true)
    {
        Socket clientSocket(accept(listenSocket, nullptr, nullptr));
        if (clientSocket == INVALID_SOCKET)
        {
#ifdef _WIN32
            errorCode = WSAGetLastError();
            assert(errorCode == WSAEWOULDBLOCK);
#elifdef linux
            errorCode = errno;
            assert(errorCode == EWOULDBLOCK);
#endif
        }
        else
        {
            result = setNonblocking(clientSocket);
            assert(result != -1);

            clients.emplace( clientNumber, std::make_unique<Client>(clientNumber, std::move(clientSocket)));
            std::cout << "[접속] 클라이언트 " << clientNumber << std::endl;
            clientNumber++;
        }

        if (!sendQueue.empty())
        {
            Message& message = sendQueue.front();
            if (message.to == nullptr)
            {
                sendQueue.pop();
            }
            else
            {
                char* pSendBuffer = (currentSent < HEADER_SIZE ? reinterpret_cast<char*>(&message) : message.body.get()) + currentSent;
                int lengthToSend = (currentSent < HEADER_SIZE ? HEADER_SIZE : message.headerBodySize) - currentSent;
                result = send(message.to->GetSocketHandle(), pSendBuffer, lengthToSend, 0);
                switch (handleResult(result, errorCode))
                {
                    case IOResult::SUCCESSFUL:
                        currentSent += result;
                        if (currentSent == HEADER_SIZE + message.headerBodySize)
                        {
                            currentSent = 0;
                            sendQueue.pop();
                        }
                    case IOResult::WOULD_BLOCK:
                        break;
                    case IOResult::FATAL_ERROR:
                        std::cerr << "[에러] 클라이언트 " << message.to->GetConnectionId() << "에게 데이터를 전송하던 도중 에러가 발생하였습니다: 에러 코드" << errorCode << "." << std::endl;
                    case IOResult::DISCONNECTED:
                        std::cout << "[접속 해제] 클라이언트 " << message.to->GetConnectionId() << std::endl;
                        clients.erase(message.to->GetConnectionId());
                        currentSent = 0;
                        sendQueue.pop();
                        break;
                }
            }
        }

        for (auto iter = clients.begin(); iter != clients.end();)
        {
            auto pClient = iter->second.get();
            if (pClient == nullptr)
            {
                clients.erase(iter++);
                continue;
            }

            char* pReceiveBuffer = pClient->receiveBuffer + pClient->currentReceived;
            int sizeToReceive = pClient->totalSizeToReceive - pClient->currentReceived;
            result = recv(pClient->GetSocketHandle(), pReceiveBuffer, sizeToReceive, 0);
            switch (handleResult(result, errorCode))
            {
                case IOResult::SUCCESSFUL:
                    pClient->currentReceived += result;
                    if (pClient->currentReceived == pClient->totalSizeToReceive)
                    {
                        if (pClient->totalSizeToReceive == HEADER_SIZE)
                        {
                            memcpy(&pClient->totalSizeToReceive, pClient->receiveBuffer, 4);
                            memcpy(&pClient->receivingHeaderType, pClient->receiveBuffer+4, 4);
                        }
                        else
                        {
                            pClient->totalSizeToReceive = HEADER_SIZE;
                        }
                        pClient->currentReceived = 0;
                    }
                case IOResult::WOULD_BLOCK:
                    break;
                case IOResult::FATAL_ERROR:
                    std::cerr << "[에러] 클라이언트 " << pClient->GetConnectionId() << "로부터 데이터 수신 중 에러가 발생하였습니다: 에러 코드 " << errorCode << "." << std::endl;
                case IOResult::DISCONNECTED:
                    std::cout << "[접속 해제] 클라이언트 " << pClient->GetConnectionId() << std::endl;
                    clients.erase(iter++);
                    break;
            }
        }
    }

    return 0;
}
