//
// Created by floweryclover on 2024-01-30.
//

#include "GameRoom.h"
#include "GameData.h"

GameRoom::GameRoom(unsigned int roomId) noexcept : mRoomId(roomId)
{

}

GameRoom::GameRoom(GameRoom &&rhs) noexcept : GameRoom(rhs.mRoomId)
{

}

void GameRoom::OnPlayerJoined(ConnectionId id) noexcept
{
    mJoinedPlayers.insert(id);
}

void GameRoom::OnPlayerLeft(ConnectionId id) noexcept
{
    mJoinedPlayers.erase(id);
}