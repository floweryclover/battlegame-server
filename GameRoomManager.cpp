//
// Created by floweryclover on 2024-01-30.
//

#include "GameRoomManager.h"
#include "GameData.h"
#include "BattleGameServer.h"
#include "ClientManager.h"

bool GameRoomManager::JoinPlayer(ConnectionId playerId, GameRoomId roomId)
{
    if (!BattleGameServer::GetConstInstance().GetConstClientManager().IsClientExists(playerId) // 해당 클라이언트가 지금 접속해 있는지
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