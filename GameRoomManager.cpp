//
// Created by floweryclover on 2024-01-30.
//

#include "GameRoomManager.h"
#include "GameData.h"
#include "BattleGameServer.h"
#include "ClientManager.h"

GameRoomManager::GameRoomManager() noexcept : mNewRoomId(GameRoom::ROOM_MATCHMAKING+1)
{
    mGameRooms.emplace(GameRoom::ROOM_MAINMENU, GameRoom(GameRoom::ROOM_MAINMENU));
    mGameRooms.emplace(GameRoom::ROOM_MATCHMAKING, GameRoom(GameRoom::ROOM_MATCHMAKING));
}

void GameRoomManager::Tick()
{
    auto matchMakePlayer = [this](ConnectionId playerId)
    {
        if (CheckIfPlayerNotValid(playerId)
            || mRoomOfPlayers[playerId] != GameRoom::ROOM_MATCHMAKING)
        {
            return;
        }

        for (auto& roomPair : mGameRooms)
        {
            if (roomPair.first == GameRoom::ROOM_MATCHMAKING || roomPair.first == GameRoom::ROOM_MAINMENU)
            {
                continue;
            }

            if (!roomPair.second.IsFull())
            {
                JoinPlayer(playerId, roomPair.first);
                return;
            }
        }

        mGameRooms.emplace(mNewRoomId, GameRoom(mNewRoomId));
        JoinPlayer(playerId, mNewRoomId);
        mNewRoomId++;
    };

    if (!mMatchMakingPlayers.empty())
    {
        auto top = mMatchMakingPlayers.front();
        mMatchMakingPlayers.pop();
        matchMakePlayer(top);
    }
}

bool GameRoomManager::CheckIfPlayerNotValid(ConnectionId id)
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

    mGameRooms.at(mRoomOfPlayers[playerId]).OnPlayerLeft(playerId);
    mGameRooms.at(roomId).OnPlayerJoined(playerId);
    mRoomOfPlayers[playerId] = roomId;
    return true;
}

bool GameRoomManager::StartMatchMaking(ConnectionId playerId)
{
    if (CheckIfPlayerNotValid(playerId)
    || mRoomOfPlayers[playerId] != GameRoom::ROOM_MAINMENU)
    {
        return false;
    }

    JoinPlayer(playerId, GameRoom::ROOM_MATCHMAKING);
    mMatchMakingPlayers.emplace(playerId);
    return true;
}

void GameRoomManager::StopMatchMaking(ConnectionId playerId)
{
    if (CheckIfPlayerNotValid(playerId)
    || mRoomOfPlayers[playerId] != GameRoom::ROOM_MATCHMAKING)
    {
        return;
    }

    JoinPlayer(playerId, GameRoom::ROOM_MAINMENU);
}

void GameRoomManager::OnPlayerConnected(ConnectionId id)
{
    mRoomOfPlayers.emplace(id, GameRoom::ROOM_MAINMENU);
}

void GameRoomManager::OnPlayerDisconnected(ConnectionId id)
{
    mGameRooms.at(mRoomOfPlayers[id]).OnPlayerLeft(id);
    mRoomOfPlayers.erase(id);
}