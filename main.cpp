#include <iostream>
#include "GameData.h"

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
    GameData::Initialize(argv[1], atoi(argv[2]));
    while (true)
    {
        GameData::GetInstance().GetClientManager().ProcessNetworkIoOnce();
    }

    return 0;
}
