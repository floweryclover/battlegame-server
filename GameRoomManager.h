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
    explicit GameRoomManager() noexcept;
    ~GameRoomManager() = default;
    static constexpr GameRoomId ROOM_MAINMENU = 0;
    static constexpr GameRoomId ROOM_MATCHMAKING = 1;
    bool JoinPlayer(ConnectionId playerId, GameRoomId roomId);
private:
    GameRoomId mNewRoomId;
    std::map<ConnectionId, GameRoomId> mRoomOfPlayers;
    std::map<ConnectionId, GameRoom> mGameRooms;
};


#endif //BATTLEGAME_SERVER_GAMEROOMMANAGER_H
