//
// Created by floweryclover on 2024-01-30.
//

#include "GameData.h"
#include "Message.h"
#include "Context.h"
#include "CtsRpc.h"
#include <iostream>
#include <cassert>

std::unique_ptr<GameData> GameData::spGameDataInstance = nullptr;

void GameData::Initialize(const char *listenAddress, unsigned short listenPort)
{
    assert(spGameDataInstance == nullptr);
    spGameDataInstance = std::make_unique<GameData>(listenAddress, listenPort);
}

GameData::GameData(const char* listenAddress, unsigned short listenPort) : mClientManager(listenAddress, listenPort)
{

}

GameData& GameData::GetInstance()
{
    assert(spGameDataInstance != nullptr);
    return *spGameDataInstance;
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