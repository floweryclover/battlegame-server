//
// Created by floweryclover on 2024-01-30.
//

#ifndef BATTLEGAME_SERVER_GAMEROOMMANAGER_H
#define BATTLEGAME_SERVER_GAMEROOMMANAGER_H

#include <map>
#include <queue>
#include "GameRoom.h"

using SerializedEndpoint = unsigned long long;
using ClientId = SerializedEndpoint;
using GameRoomId = unsigned int;

class GameRoomManager {
public:
    explicit GameRoomManager() noexcept;
    ~GameRoomManager() = default;

    bool JoinPlayer(ClientId clientId, GameRoomId roomId);
    bool StartMatchMaking(ClientId clientId);
    void StopMatchMaking(ClientId clientId);

    void OnPlayerConnected(ClientId clientId);
    void OnPlayerDisconnected(ClientId clientId);

    void Tick();

private:
    GameRoomId mNewRoomId;
    std::map<ClientId, GameRoomId> mRoomOfPlayers;
    std::map<GameRoomId, GameRoom> mGameRooms;
    std::queue<ClientId> mMatchMakingPlayers;

    static bool CheckIfPlayerNotValid(ClientId clientId);
};


#endif //BATTLEGAME_SERVER_GAMEROOMMANAGER_H
