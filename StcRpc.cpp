//
// Created by floweryclover on 2024-01-27.
//

#include "StcRpc.h"

StcRpc::StcRpc(std::function<void(unsigned int, Message&&)> sendEnqueueFunction) : SendEnqueue(std::move(sendEnqueueFunction)) {}

void StcRpc::RequestEnterNickname(unsigned int connectionId) const
{
    Message message(0, STC_REQUEST_ENTER_NICKNAME, nullptr);
    SendEnqueue(connectionId, std::move(message));
}