//
// Created by floweryclover on 2024-01-30.
//

#include "GameData.h"
#include "GameRoomManager.h"
#include "CtsRpc.h"

GameData::GameData() noexcept : mpGameRoomManager(std::make_unique<GameRoomManager>()) {}

void GameData::OnPlayerConnected(ConnectionId id)
{
    mNicknames.emplace(id, std::string("플레이어"));
    mpGameRoomManager->OnPlayerConnected(id);
}

void GameData::OnPlayerDisconnected(ConnectionId id)
{
    mNicknames.erase(id);
    mpGameRoomManager->OnPlayerDisconnected(id);
}

bool GameData::SetPlayerNickname(ConnectionId id, const std::string& nickname)
{
    if (mNicknames.contains(id))
    {
        mNicknames[id] = nickname;
        return true;
    }
    else
    {
        return false;
    }
}