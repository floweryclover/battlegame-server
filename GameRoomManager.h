//
// Created by floweryclover on 2024-01-30.
//

#ifndef BATTLEGAME_SERVER_GAMEROOMMANAGER_H
#define BATTLEGAME_SERVER_GAMEROOMMANAGER_H

#include <map>
#include <queue>
#include "GameRoom.h"

using ConnectionId = unsigned int;
using GameRoomId = unsigned int;

class GameRoomManager {
public:
    explicit GameRoomManager() noexcept;
    ~GameRoomManager() = default;

    bool JoinPlayer(ConnectionId playerId, GameRoomId roomId);
    bool StartMatchMaking(ConnectionId playerId);
    void StopMatchMaking(ConnectionId playerId);

    void OnPlayerConnected(ConnectionId id);
    void OnPlayerDisconnected(ConnectionId id);

    void Tick();

private:
    GameRoomId mNewRoomId;
    std::map<ConnectionId, GameRoomId> mRoomOfPlayers;
    std::map<GameRoomId, GameRoom> mGameRooms;
    std::queue<ConnectionId> mMatchMakingPlayers;

    static bool CheckIfPlayerNotValid(ConnectionId id);
};


#endif //BATTLEGAME_SERVER_GAMEROOMMANAGER_H
