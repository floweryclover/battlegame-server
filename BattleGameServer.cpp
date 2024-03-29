//
// Created by floweryclover on 2024-01-31.
//

#include "BattleGameServer.h"
#include "GameData.h"
#include "ClientManager.h"
#include "GameRoomManager.h"
#include "StcRpc.h"
#include "CtsRpc.h"
#include <cassert>

std::unique_ptr<BattleGameServer> BattleGameServer::spSingleton = nullptr;

void BattleGameServer::Initialize(const char* listenAddress, unsigned short listenPort) noexcept
{
    assert(spSingleton == nullptr);
    spSingleton = std::unique_ptr<BattleGameServer>(new BattleGameServer());

    spSingleton->mpClientManager = std::make_unique<ClientManager>(listenAddress, listenPort);
    spSingleton->mpGameData = std::make_unique<GameData>();
    spSingleton->mpStcRpc = std::make_unique<StcRpc>();
    spSingleton->mpCtsRpc = std::make_unique<CtsRpc>();
}

void BattleGameServer::InvokeTick()
{
    mpGameData->GetGameRoomManager().Tick();
    mpClientManager->Tick();
}