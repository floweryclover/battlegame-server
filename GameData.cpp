//
// Created by floweryclover on 2024-01-30.
//

#include "GameData.h"

GameData& GameData::GetInstance()
{
    static GameData instance;
    return instance;
}

void GameData::OnPlayerDisconnected(ConnectionId id)
{
    mNicknames.erase(id);
}

void GameData::OnPlayerConnected(ConnectionId id)
{
    mNicknames.emplace(id, std::string("플레이어"));
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
