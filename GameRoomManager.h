//
// Created by floweryclover on 2024-01-30.
//

#ifndef BATTLEGAME_SERVER_GAMEROOMMANAGER_H
#define BATTLEGAME_SERVER_GAMEROOMMANAGER_H

#include <map>
#include "GameRoom.h"

using ConnectionId = unsigned int;
using GameRoomId = unsigned int;

class GameRoomManager {
public:
    bool JoinPlayer(ConnectionId playerId, GameRoomId roomId);
private:
    std::map<ConnectionId, GameRoomId> mRoomOfPlayers;
    std::map<ConnectionId, GameRoom> mGameRooms;
};


#endif //BATTLEGAME_SERVER_GAMEROOMMANAGER_H
