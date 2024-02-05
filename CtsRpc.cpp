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

void CtsRpc::HandleMessage(const Context& context, const Message& message) const
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
    }
}

void CtsRpc::OnRequestMatchMaking(const Context& context) const
{
    BattleGameServer::GetInstance()
    .GetGameData()
    .GetGameRoomManager()
    .StartMatchMaking(context.GetClientId());
}

void CtsRpc::OnMoveCharacter(const Context &context, double x, double y, double z) const
{
    std::cout << "클라이언트 " << context.GetClientId() << " 이동: " << x << ", " << y << ", " << z << std::endl;
}

void CtsRpc::OnEnterNickname(const Context &context, std::string &&nickname) const
{
    BattleGameServer::GetInstance().GetGameData().SetPlayerNickname(context.GetClientId(), nickname);
}