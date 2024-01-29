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
#include "CtsRpc.h"
#include "StcRpc.h"
#include "Context.h"
#include "GameRoom.h"

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
    int option = 1;
    int result = setsockopt(listenSocket.AsHandle(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    assert(result != -1);

    struct sockaddr_in sockAddrIn {};
    memset(&sockAddrIn, 0, sizeof(struct sockaddr_in));
    result = inet_pton(AF_INET, argv[1], &sockAddrIn.sin_addr);
    assert(result == 1);
    sockAddrIn.sin_family = AF_INET;
    sockAddrIn.sin_port = htons(atoi(argv[2]));
    result = bind(listenSocket.AsHandle(), (struct sockaddr*)&sockAddrIn, sizeof(sockAddrIn));
    assert(result != -1);

    result = listen(listenSocket.AsHandle(), 8);
    assert(result != -1);

    result = setNonblocking(listenSocket.AsHandle());
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
                        return IoResult::IO_WOULD_BLOCK;
                    }
                    else
                    {
                        return IoResult::IO_FATAL_ERROR;
                    }
                }
                else if (ioReturnValue == 0)
                {
                    return IoResult::IO_DISCONNECTED;
                }
                else
                {
                    return IoResult::IO_SUCCESSFUL;
                }
            };

    CtsRpc ctsRpc;
    StcRpc stcRpc;
    int errorCode;
    std::queue<std::pair<int, Message>> sendQueue;
    std::map<unsigned int, std::unique_ptr<Client>> clients;
    unsigned int clientNumber = 0;
    int currentSent = 0;

    std::map<unsigned int, std::unique_ptr<GameRoom>> gameRooms;

    while (true)
    {
        Socket clientSocket(accept(listenSocket.AsHandle(), nullptr, nullptr));
        if (clientSocket.AsHandle() == INVALID_SOCKET)
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
            result = setNonblocking(clientSocket.AsHandle());
            assert(result != -1);

            clients.emplace( clientNumber, std::make_unique<Client>(clientNumber, std::move(clientSocket)));
            std::cout << "[접속] 클라이언트 " << clientNumber << std::endl;
            clientNumber++;
        }

        if (!sendQueue.empty())
        {
            auto& messagePair = sendQueue.front();
            if (clients.find(messagePair.first) == clients.end())
            {
                sendQueue.pop();
            }
            else
            {
                const Client* const pTargetClient = clients[messagePair.first].get();
                const Message& message = messagePair.second;

                const char* pSendBuffer = (currentSent < HEADER_SIZE ? reinterpret_cast<const char*>(&message) : message.mBodyBuffer) + currentSent;
                int lengthToSend = (currentSent < HEADER_SIZE ? HEADER_SIZE : message.mHeaderBodySize) - currentSent;
                result = send(pTargetClient->GetSocket().AsHandle(), pSendBuffer, lengthToSend, 0);
                switch (handleResult(result, errorCode))
                {
                    case IoResult::IO_SUCCESSFUL:
                        currentSent += result;
                        if (currentSent == HEADER_SIZE + message.mHeaderBodySize)
                        {
                            currentSent = 0;
                            sendQueue.pop();
                        }
                    case IoResult::IO_WOULD_BLOCK:
                        break;
                    case IoResult::IO_FATAL_ERROR:
                        std::cerr << "[에러] 클라이언트 " << pTargetClient->GetConnectionId() << "에게 데이터를 전송하던 도중 에러가 발생하였습니다: 에러 코드" << errorCode << "." << std::endl;
                    case IoResult::IO_DISCONNECTED:
                        std::cout << "[접속 해제] 클라이언트 " << pTargetClient->GetConnectionId() << std::endl;
                        clients.erase(pTargetClient->GetConnectionId());
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
            auto receiveResult = pClient->Receive();
            if (receiveResult.has_value())
            {
                if (receiveResult.value().has_value())
                {
                    Context context(pClient->GetConnectionId());
                    ctsRpc.HandleMessage(context, *receiveResult.value().value());
                }
            }
            else
            {
                if (receiveResult.error().has_value())
                {
                    std::cerr << "[에러] 클라이언트 " << pClient->GetConnectionId() << " 에게서 데이터를 수신하던 도중 에러가 발생하였습니다: 에러 코드 " << receiveResult.error().value() << "." << std::endl;
                }
                std::cout << "[접속 해제] 클라이언트 " << pClient->GetConnectionId() << std::endl;
                clients.erase(iter++);
            }
        }
    }

    return 0;
}
