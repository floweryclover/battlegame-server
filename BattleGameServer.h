//
// Created by floweryclover on 2024-01-31.
//

#ifndef BATTLEGAME_SERVER_BATTLEGAMESERVER_H
#define BATTLEGAME_SERVER_BATTLEGAMESERVER_H

#include <memory>

class ClientManager;
class GameData;
class StcRpc;
class CtsRpc;

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

    inline StcRpc& GetStcRpc() noexcept { return const_cast<StcRpc&>(GetConstStcRpc()); }
    inline const StcRpc& GetConstStcRpc() const noexcept { return *this->mpStcRpc; }

    inline CtsRpc& GetCtsRpc() noexcept { return const_cast<CtsRpc&>(GetConstCtsRpc()); }
    inline const CtsRpc& GetConstCtsRpc() const noexcept { return *this->mpCtsRpc; }

    void InvokeTick();
private:
    BattleGameServer() = default;
    static std::unique_ptr<BattleGameServer> spSingleton;
    std::unique_ptr<ClientManager> mpClientManager;
    std::unique_ptr<GameData> mpGameData;
    std::unique_ptr<StcRpc> mpStcRpc;
    std::unique_ptr<CtsRpc> mpCtsRpc;
};


#endif //BATTLEGAME_SERVER_BATTLEGAMESERVER_H
