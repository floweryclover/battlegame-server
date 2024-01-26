#include <iostream>
#include <cassert>
#include <cstring>
#include "Socket.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "Wsa.h"
#pragma comment(lib, "ws2_32.lib")
#endif

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <listen-address> <listen-port>" << std::endl;
        return 1;
    }
#ifdef _WIN32
    Wsa wsa;
#endif
    Socket listenSocket(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));

    struct sockaddr_in sockAddrIn;
    memset(&sockAddrIn, 0, sizeof(struct sockaddr_in));
    int result = inet_pton(AF_INET, argv[1], &sockAddrIn.sin_addr);
    assert(result == 1);
    sockAddrIn.sin_family = AF_INET;
    sockAddrIn.sin_port = htons(atoi(argv[2]));
    result = bind(listenSocket, (struct sockaddr*)&sockAddrIn, sizeof(sockAddrIn));
    assert(result != -1);

    result = listen(static_cast<SOCKET>(listenSocket), 8);
    assert(result != -1);

    Socket clientSocket(accept(listenSocket, nullptr, nullptr));
    assert(clientSocket != -1);

    int headerType = 0;
    int headerBodySize = 14;
    const char* msg = "Hello Client!";
    send(clientSocket, reinterpret_cast<char*>(&headerType), 4, 0);
    send(clientSocket, reinterpret_cast<char*>(&headerBodySize), 4, 0);
    send(clientSocket, msg, 14, 0);

    headerType = 0;
    headerBodySize = 11;
    msg = "Hi Client!";
    send(clientSocket, reinterpret_cast<char*>(&headerType), 4, 0);
    send(clientSocket, reinterpret_cast<char*>(&headerBodySize), 4, 0);
    send(clientSocket, msg, 11, 0);

    return 0;
}