//
// Created by floweryclover on 2024-01-27.
//

#include "CtsRpc.h"
#include "StcRpc.h"
#include "GameData.h"
#include "BattleGameServer.h"
#include "GameRoomManager.h"
#include <iostream>

void CtsRpc::HandleMessage(const Context& context, const Message& message) const
{
    switch (message.mHeaderMessageType)
    {
        case CtsRpc::CTS_REQUEST_MATCHMAKING:
            OnRequestMatchMaking(context);
            break;
    }
}

void CtsRpc::OnRequestMatchMaking(const Context& context) const
{
    BattleGameServer::GetInstance()
    .GetGameData()
    .GetGameRoomManager()
    .StartMatchMaking(context.GetClientId());
}

void CtsRpc::OnEnterNickname(const Context &context, std::string &&nickname) const
{
    BattleGameServer::GetInstance().GetGameData().SetPlayerNickname(context.GetClientId(), nickname);
}