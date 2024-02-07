//
// Created by floweryclover on 2024-01-27.
//

#include "CtsRpc.h"
#include "StcRpc.h"
#include "GameData.h"
#include "BattleGameServer.h"
#include "ClientManager.h"
#include "GameRoomManager.h"
#include <iostream>

void CtsRpc::HandleMessage(const Context& context, const Message& message) const noexcept
{
    switch (message.mHeaderMessageType)
    {
        case CtsRpc::CTS_REQUEST_MATCHMAKING:
            OnRequestMatchMaking(context);
            break;
        case CtsRpc::CTS_MOVE_CHARACTER:
            double x,y,z;
            memcpy(&x, message.mpBodyBuffer.get(), 8);
            memcpy(&y, message.mpBodyBuffer.get()+8, 8);
            memcpy(&z, message.mpBodyBuffer.get()+16, 8);
            OnMoveCharacter(context, x, y, z);
            break;
        case CtsRpc::CTS_ACK_UDP_TOKEN:
            // 헤더 주석 참고
            break;
        case CtsRpc::CTS_NOTIFY_BATTLEGAME_PREPARED:
            OnNotifyBattleGamePrepared(context);
            break;
    }
}

void CtsRpc::OnRequestMatchMaking(const Context& context) const noexcept
{
    BattleGameServer::GetInstance()
    .GetGameData()
    .GetGameRoomManager()
    .StartMatchMaking(context.GetClientId());
}

void CtsRpc::OnMoveCharacter(const Context &context, double x, double y, double z) const noexcept
{

}

void CtsRpc::OnEnterNickname(const Context &context, std::string &&nickname) const noexcept
{
    BattleGameServer::GetInstance().GetGameData().SetPlayerNickname(context.GetClientId(), nickname);
}

void CtsRpc::OnNotifyBattleGamePrepared(const Context &context) const noexcept
{
    auto gameRoomOption = BattleGameServer::GetInstance().GetGameData().GetGameRoomManager().GetPlayerJoinedRoomId(context.GetClientId());
    if (!gameRoomOption.has_value())
    {
        return;
    }
    BattleGameServer::GetInstance().GetGameData().GetGameRoomManager().GetGameRoom(gameRoomOption.value())->InvokeOnPlayerPrepared(context.GetClientId());
}