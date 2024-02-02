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
    auto matchMakePlayer = [this](ClientId clientId)
    {
        if (CheckIfPlayerNotValid(clientId)
            || mRoomOfPlayers[clientId] != GameRoom::ROOM_MATCHMAKING)
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
                JoinPlayer(clientId, roomPair.first);
                return;
            }
        }

        mGameRooms.emplace(mNewRoomId, GameRoom(mNewRoomId));
        JoinPlayer(clientId, mNewRoomId);
        mNewRoomId++;
    };

    if (!mMatchMakingPlayers.empty())
    {
        auto top = mMatchMakingPlayers.front();
        mMatchMakingPlayers.pop();
        matchMakePlayer(top);
    }
}

bool GameRoomManager::CheckIfPlayerNotValid(ClientId clientId)
{
    return !BattleGameServer::GetConstInstance().GetConstClientManager().IsClientExists(clientId);
}

bool GameRoomManager::JoinPlayer(ClientId clientId, GameRoomId roomId)
{
    if (CheckIfPlayerNotValid(clientId) // 해당 클라이언트가 지금 접속해 있는지
    || mGameRooms.find(roomId) == mGameRooms.end()) // 해당 방이 존재하는지
    {
        return false;
    }

    mGameRooms.at(mRoomOfPlayers[clientId]).OnPlayerLeft(clientId);
    mGameRooms.at(roomId).OnPlayerJoined(clientId);
    mRoomOfPlayers[clientId] = roomId;
    return true;
}

bool GameRoomManager::StartMatchMaking(ClientId clientId)
{
    if (CheckIfPlayerNotValid(clientId)
    || mRoomOfPlayers[clientId] != GameRoom::ROOM_MAINMENU)
    {
        return false;
    }

    JoinPlayer(clientId, GameRoom::ROOM_MATCHMAKING);
    mMatchMakingPlayers.emplace(clientId);
    return true;
}

void GameRoomManager::StopMatchMaking(ClientId clientId)
{
    if (CheckIfPlayerNotValid(clientId)
    || mRoomOfPlayers[clientId] != GameRoom::ROOM_MATCHMAKING)
    {
        return;
    }

    JoinPlayer(clientId, GameRoom::ROOM_MAINMENU);
}

void GameRoomManager::OnPlayerConnected(ClientId clientId)
{
    mRoomOfPlayers.emplace(clientId, GameRoom::ROOM_MAINMENU);
}

void GameRoomManager::OnPlayerDisconnected(ClientId clientId)
{
    mGameRooms.at(mRoomOfPlayers[clientId]).OnPlayerLeft(clientId);
    mRoomOfPlayers.erase(clientId);
}