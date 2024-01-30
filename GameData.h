//
// Created by floweryclover on 2024-01-30.
//

#ifndef BATTLEGAME_SERVER_GAMEDATA_H
#define BATTLEGAME_SERVER_GAMEDATA_H

#include <map>
#include <memory>
#include <string>
#include "GameRoomManager.h"
#include "ClientManager.h"

using ConnectionId = unsigned int;

class GameData {
public:
    GameData(GameData&& rhs) = delete;
    GameData(const GameData& rhs) = delete;
    GameData& operator=(const GameData& rhs) = delete;

    GameData(const char* listenAddress, unsigned short listenPort);

    static void Initialize(const char* listenAddress, unsigned short listenPort);
    static GameData& GetInstance();
    void OnPlayerConnected(ConnectionId id);
    void OnPlayerDisconnected(ConnectionId id);

    bool SetPlayerNickname(ConnectionId id, const std::string& nickname);
    inline const std::string* GetPlayerNickname(ConnectionId id) { return mNicknames.contains(id) ? &mNicknames[id] : nullptr; }

    inline GameRoomManager& GetGameRoomManager() { return mGameRoomManager; }
    inline const GameRoomManager& GetGameRoomManager() const { return mGameRoomManager; }

    inline ClientManager& GetClientManager() { return mClientManager; }
    inline const ClientManager& GetClientManager() const { return mClientManager; }
private:
    static std::unique_ptr<GameData> spGameDataInstance;
    std::map<ConnectionId, std::string> mNicknames;
    GameRoomManager mGameRoomManager;
    ClientManager mClientManager;
};


#endif //BATTLEGAME_SERVER_GAMEDATA_H
