//
// Created by floweryclover on 2024-01-30.
//

#ifndef BATTLEGAME_SERVER_GAMEDATA_H
#define BATTLEGAME_SERVER_GAMEDATA_H

#include <map>
#include <memory>
#include <string>

using SerializedEndpoint = unsigned long long;
using ClientId = SerializedEndpoint;

class GameRoomManager;

class GameData {
public:
    GameData(GameData&& rhs) = delete;
    GameData(const GameData& rhs) = delete;
    GameData& operator=(const GameData& rhs) = delete;

    explicit GameData() noexcept;
    ~GameData() = default;

    void OnPlayerConnected(ClientId clientId);
    void OnPlayerDisconnected(ClientId clientId);

    bool SetPlayerNickname(ClientId clientId, const std::string& nickname);
    inline const std::string* GetPlayerNickname(ClientId clientId) { return mNicknames.contains(clientId) ? &mNicknames[clientId] : nullptr; }

    inline GameRoomManager& GetGameRoomManager() { return const_cast<GameRoomManager&>(GetConstGameRoomManager()); }
    inline const GameRoomManager& GetConstGameRoomManager() const { return *mpGameRoomManager; }

private:
    std::map<ClientId ,std::string> mNicknames;
    std::unique_ptr<GameRoomManager> mpGameRoomManager;
};


#endif //BATTLEGAME_SERVER_GAMEDATA_H
