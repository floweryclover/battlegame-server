//
// Created by floweryclover on 2024-01-30.
//

#include "GameRoomManager.h"
#include "GameData.h"
#include "BattleGameServer.h"
#include "ClientManager.h"

GameRoomManager::GameRoomManager() noexcept : mNewRoomId(ROOM_MATCHMAKING+1) {}

bool GameRoomManager::CheckIfPlayerNotValid(ConnectionId id) const
{
    return !BattleGameServer::GetConstInstance().GetConstClientManager().IsClientExists(id);
}

bool GameRoomManager::JoinPlayer(ConnectionId playerId, GameRoomId roomId)
{
    if (CheckIfPlayerNotValid(playerId) // 해당 클라이언트가 지금 접속해 있는지
    || mGameRooms.find(roomId) == mGameRooms.end()) // 해당 방이 존재하는지
    {
        return false;
    }

    // 만약 다른 방에 속해있으면 퇴장 처리
    if (mRoomOfPlayers.find(playerId) != mRoomOfPlayers.end())
    {
        // 퇴장 처리 로직
        mRoomOfPlayers.erase(playerId);
    }

    // 입장 처리 로직
    mRoomOfPlayers.emplace(playerId, roomId);
    return true;
}

bool GameRoomManager::StartMatchMaking(ConnectionId playerId)
{
    if (CheckIfPlayerNotValid(playerId)
    || mRoomOfPlayers[playerId] != ROOM_MAINMENU)
    {
        return false;
    }

    mRoomOfPlayers[playerId] = ROOM_MATCHMAKING;
}

void GameRoomManager::StopMatchMaking(ConnectionId playerId)
{
    if (CheckIfPlayerNotValid(playerId)
    || mRoomOfPlayers[playerId] != ROOM_MATCHMAKING)
    {
        return;
    }

    mRoomOfPlayers[playerId] = ROOM_MAINMENU;
}

void GameRoomManager::OnPlayerConnected(ConnectionId id)
{
    mRoomOfPlayers.emplace(id, ROOM_MAINMENU);
}

void GameRoomManager::OnPlayerDisconnected(ConnectionId id)
{
    mRoomOfPlayers.erase(id);
}