//
// Created by floweryclover on 2024-01-30.
//

#include "GameRoom.h"

GameRoom::GameRoom(unsigned int roomId) noexcept : mRoomId(roomId)
{

}

GameRoom::GameRoom(GameRoom &&rhs) noexcept : GameRoom(rhs.mRoomId)
{

}
