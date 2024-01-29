//
// Created by floweryclover on 2024-01-27.
//

#include "CtsRpc.h"
#include "StcRpc.h"
#include <iostream>

void CtsRpc::HandleMessage(const Context& context, const Message& message) const
{
    switch (message.mHeaderMessageType)
    {
        case CtsRpc::CTS_REQUEST_JOIN_GAME:
            RequestJoinGame(context);
            break;
    }
}

void CtsRpc::RequestJoinGame(Context context) const
{
    std::cout << "Hello!" << std::endl;
    mpStcRpc->Test();
}
