//
// Created by floweryclover on 2024-01-30.
//

#ifndef BATTLEGAME_SERVER_GAMEDATA_H
#define BATTLEGAME_SERVER_GAMEDATA_H

#include <map>
#include <memory>
#include <string>

using ConnectionId = unsigned int;

class GameRoomManager;

class GameData {
public:
    GameData(GameData&& rhs) = delete;
    GameData(const GameData& rhs) = delete;
    GameData& operator=(const GameData& rhs) = delete;

    explicit GameData() noexcept;
    ~GameData() = default;

    void OnPlayerConnected(ConnectionId id);
    void OnPlayerDisconnected(ConnectionId id);

    bool SetPlayerNickname(ConnectionId id, const std::string& nickname);
    inline const std::string* GetPlayerNickname(ConnectionId id) { return mNicknames.contains(id) ? &mNicknames[id] : nullptr; }

    inline GameRoomManager& GetGameRoomManager() { return const_cast<GameRoomManager&>(GetConstGameRoomManager()); }
    inline const GameRoomManager& GetConstGameRoomManager() const { return *mpGameRoomManager; }

private:
    std::map<ConnectionId, std::string> mNicknames;
    std::unique_ptr<GameRoomManager> mpGameRoomManager;
};


#endif //BATTLEGAME_SERVER_GAMEDATA_H
