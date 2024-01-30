//
// Created by floweryclover on 2024-01-30.
//

#ifndef BATTLEGAME_SERVER_GAMEDATA_H
#define BATTLEGAME_SERVER_GAMEDATA_H

#include <map>
#include <string>
#include <optional>
#include <expected>

using ConnectionId = unsigned int;

class GameData {
public:
    GameData(GameData&& rhs) = delete;
    GameData(const GameData& rhs) = delete;
    GameData& operator=(const GameData& rhs) = delete;

    static GameData& GetInstance();
    void OnPlayerConnected(ConnectionId id);
    void OnPlayerDisconnected(ConnectionId id);

    bool SetPlayerNickname(ConnectionId id, const std::string& nickname);
    inline const std::string* GetPlayerNickname(ConnectionId id) { return mNicknames.contains(id) ? &mNicknames[id] : nullptr; }
private:
    GameData() = default;
    std::map<ConnectionId, std::string> mNicknames;
};


#endif //BATTLEGAME_SERVER_GAMEDATA_H
