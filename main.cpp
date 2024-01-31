#include <iostream>
#include "BattleGameServer.h"
#include "ClientManager.h"

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
        BattleGameServer::GetInstance().GetClientManager().ProcessNetworkIoOnce();
    }

    return 0;
}
