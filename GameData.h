//
// Created by floweryclover on 2024-01-30.
//

#ifndef BATTLEGAME_SERVER_GAMEDATA_H
#define BATTLEGAME_SERVER_GAMEDATA_H

#include <map>
#include <string>
#include "GameRoom.h"
#include "Client.h"

using ConnectionId = unsigned int;
using GameRoomId = unsigned int;
class CtsRpc;

class GameData {
public:
    GameData(GameData&& rhs) = delete;
    GameData(const GameData& rhs) = delete;
    GameData& operator=(const GameData& rhs) = delete;

    static GameData& GetInstance();
    void OnPlayerConnected(ConnectionId id, Client&& client);
    void OnPlayerDisconnected(ConnectionId id);

    bool SetPlayerNickname(ConnectionId id, const std::string& nickname);
    inline const std::string* GetPlayerNickname(ConnectionId id) { return mNicknames.contains(id) ? &mNicknames[id] : nullptr; }
    inline bool IsClientExists(ConnectionId id) { return mClients.find(id) != mClients.end(); }
    inline const Client* GetClient(ConnectionId id) { return mClients.contains(id) ? &mClients.at(id) : nullptr; }
    bool JoinRandomGameRoom(ConnectionId id);

    void AllClientReceive(CtsRpc& ctsRpc);
private:
    GameData() = default;
    std::map<unsigned int, Client> mClients;
    std::map<ConnectionId, std::string> mNicknames;
    std::map<GameRoomId, GameRoom> mGameRooms;
};


#endif //BATTLEGAME_SERVER_GAMEDATA_H
