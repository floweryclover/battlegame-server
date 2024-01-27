#include <iostream>
#include <cassert>
#include <cstring>

#include "Socket.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "Wsa.h"
#pragma comment(lib, "ws2_32.lib")
#elifdef linux
#include <sys/socket.h>
#include <arpa/inet.h>
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
    SetConsoleOutputCP(CP_UTF8);
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

    result = listen(listenSocket, 8);
    assert(result != -1);

    Socket clientSocket(accept(listenSocket, nullptr, nullptr));
    assert(clientSocket != -1);

    int bytesTransferred = 0;
    int headerBodySize;
    while (bytesTransferred < 4)
    {
        result = recv(clientSocket, reinterpret_cast<char*>(&headerBodySize + bytesTransferred), 4-bytesTransferred, 0);
        assert(result != -1);
        if (result == 0)
        {
            return 1;
        }
        bytesTransferred += result;
    }
    std::cout << "HeaderBodySize: " << headerBodySize << std::endl;

    bytesTransferred = 0;
    int headerMessageType;
    while (bytesTransferred < 4)
    {
        result = recv(clientSocket, reinterpret_cast<char*>(&headerMessageType + bytesTransferred), 4-bytesTransferred, 0);
        assert(result != -1);
        if (result == 0)
        {
            return 1;
        }
        bytesTransferred += result;
    }
    std::cout << "HeaderMessageType: " << headerMessageType << std::endl;

    bytesTransferred = 0;
    char* nickname = new char[headerBodySize];
    while (bytesTransferred < headerBodySize)
    {
        result = recv(clientSocket, reinterpret_cast<char*>(nickname + bytesTransferred), headerBodySize-bytesTransferred, 0);
        assert(result != -1);
        if (result == 0)
        {
            return 1;
        }
        bytesTransferred += result;
    }
    std::wcout << "nickname: " << nickname << std::endl;
    delete[] nickname;

    return 0;
}