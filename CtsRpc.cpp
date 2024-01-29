//
// Created by floweryclover on 2024-01-27.
//

#include "CtsRpc.h"
#include <iostream>

void CtsRpc::HandleMessage(const Context& context, const Message& message) const
{
    switch (message.mHeaderMessageType)
    {
        case CtsRpc::LOGIN:
            OnLogin(context, std::string(message.mBodyBuffer, message.mHeaderBodySize));
            break;
    }
}

void CtsRpc::OnLogin(const Context& context, std::string&& nickname) const
{
    std::cout << "클라이언트 " << context.GetConnectionId() << " 닉네임 설정: " << nickname << std::endl;
}