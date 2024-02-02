//
// Created by floweryclover on 2024-01-30.
//

#include "GameRoom.h"
#include "GameData.h"
#include "BattleGameServer.h"
#include "StcRpc.h"
#include <iostream>

GameRoom::GameRoom(unsigned int roomId) noexcept : mRoomId(roomId)
{
    this->mMaxPlayerCount = 4;
}

GameRoom::GameRoom(GameRoom &&rhs) noexcept : GameRoom(rhs.mRoomId)
{

}

void GameRoom::OnPlayerJoined(ClientId clientId) noexcept
{
    mJoinedPlayers.insert(clientId);
    if (mRoomId == ROOM_MAINMENU)
    {
        return;
    }
    else if (mRoomId == ROOM_MATCHMAKING)
    {
        std::cout << "플레이어 " << clientId << " 매치메이킹 시작" << std::endl;
        return;
    }

    std::cout << "플레이어 " << clientId << " 방 " << mRoomId << " 참여" << std::endl;
    BattleGameServer::GetInstance()
    .GetConstStcRpc()
    .JoinedGameRoom(clientId);
}

void GameRoom::OnPlayerLeft(ClientId clientId) noexcept
{
    mJoinedPlayers.erase(clientId);
    if (mRoomId == ROOM_MAINMENU)
    {
        return;
    }
    else if (mRoomId == ROOM_MATCHMAKING)
    {
        std::cout << "플레이어 " << clientId << " 매치메이킹 종료" << std::endl;
        return;
    }

    std::cout << "플레이어 " << clientId << " 방 " << mRoomId << " 퇴장" << std::endl;
    BattleGameServer::GetInstance()
    .GetConstStcRpc()
    .DisconnectedFromGame(clientId);
}