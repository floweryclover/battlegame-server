#include <iostream>
#include <thread>
#include <chrono>
#include "BattleGameServer.h"

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
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
