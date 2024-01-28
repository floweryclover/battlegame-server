//
// Created by floweryclover on 2024-01-27.
//

#include "CtsRpc.h"
#include <iostream>

void CtsRpc::HandleMessage(const Client& whose) const
{
    // Client와 context를 분리할 수 있다면?
    switch (whose.mLastReceivedHeaderType)
    {
        case CtsRpc::LOGIN:
            OnLogin(whose, std::string(whose.mReceiveBuffer, whose.mTotalSizeToReceive));
            break;
    }
}

void CtsRpc::OnLogin(const Client& from, std::string&& nickname) const
{
    std::cout << "클라이언트 " << from.GetConnectionId() << " 닉네임 설정: " << nickname << std::endl;
}