#include <iostream>
#include <thread>
#include <chrono>
#include "BattleGameServer.h"

#ifdef _WIN32
#include <WinSock2.h>
#include "Wsa.h"
#endif
int main(int argc, char* argv[])
{
#ifdef _WIN32
    Wsa wsa;
    SetConsoleOutputCP(CP_UTF8);
#endif
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <listen-address> <listen-port>" << std::endl;
        return 1;
    }
    BattleGameServer::Initialize(argv[1], atoi(argv[2]));
    while (true)
    {
        BattleGameServer::GetInstance().InvokeTick();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
