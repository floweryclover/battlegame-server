//
// Created by floweryclover on 2024-01-27.
//

#include "CtsRpc.h"
#include "StcRpc.h"
#include "GameData.h"
#include <iostream>

void CtsRpc::HandleMessage(const Context& context, const Message& message) const
{
    switch (message.mHeaderMessageType)
    {
        case CtsRpc::CTS_REQUEST_JOIN_GAME:
            OnRequestJoinGame(context);
            break;
    }
}

void CtsRpc::OnRequestJoinGame(const Context& context) const
{

}

void CtsRpc::OnEnterNickname(const Context &context, std::string &&nickname) const
{
    GameData::GetInstance().SetPlayerNickname(context.GetConnectionId(), nickname);
}