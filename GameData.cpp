//
// Created by floweryclover on 2024-01-30.
//

#include "GameData.h"
#include "GameRoomManager.h"
#include "CtsRpc.h"

GameData::GameData() noexcept : mpGameRoomManager(std::make_unique<GameRoomManager>()) {}

void GameData::OnPlayerConnected(ClientId clientId)
{
    mNicknames.emplace(clientId, std::string("플레이어"));
    mpGameRoomManager->OnPlayerConnected(clientId);
}

void GameData::OnPlayerDisconnected(ClientId clientId)
{
    mNicknames.erase(clientId);
    mpGameRoomManager->OnPlayerDisconnected(clientId);
}

bool GameData::SetPlayerNickname(ClientId clientId, const std::string& nickname)
{
    if (mNicknames.contains(clientId))
    {
        mNicknames[clientId] = nickname;
        return true;
    }
    else
    {
        return false;
    }
}