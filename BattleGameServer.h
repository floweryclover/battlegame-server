//
// Created by floweryclover on 2024-01-31.
//

#ifndef BATTLEGAME_SERVER_BATTLEGAMESERVER_H
#define BATTLEGAME_SERVER_BATTLEGAMESERVER_H

#include <memory>

class ClientManager;
class GameData;
class StcRpc;

class BattleGameServer {
public:
    BattleGameServer(const BattleGameServer&) = delete;
    BattleGameServer(BattleGameServer&&) = delete;
    BattleGameServer& operator=(BattleGameServer&&) = delete;

    ~BattleGameServer() = default;

    inline static BattleGameServer& GetInstance() noexcept { return const_cast<BattleGameServer&>(GetConstInstance()); };
    inline static const BattleGameServer& GetConstInstance() noexcept { return *spSingleton; };
    static void Initialize(const char* listenAddress, unsigned short listenPort) noexcept;

    inline ClientManager& GetClientManager() noexcept { return const_cast<ClientManager&>(GetConstClientManager()); }
    inline const ClientManager& GetConstClientManager() const noexcept { return *this->mpClientManager; }

    inline GameData& GetGameData() noexcept { return const_cast<GameData&>(GetConstGameData()); }
    inline const GameData& GetConstGameData() const noexcept { return *this->mpGameData; }

    void Tick();
private:
    BattleGameServer() = default;
    static std::unique_ptr<BattleGameServer> spSingleton;
    std::unique_ptr<ClientManager> mpClientManager;
    std::unique_ptr<GameData> mpGameData;
    std::unique_ptr<StcRpc> mpStcRpc;
};


#endif //BATTLEGAME_SERVER_BATTLEGAMESERVER_H
