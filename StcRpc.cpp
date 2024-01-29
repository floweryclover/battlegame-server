//
// Created by floweryclover on 2024-01-27.
//

#include "StcRpc.h"

StcRpc::StcRpc(std::function<void(int, Message&&)> sendEnqueueFunction) : SendEnqueue(std::move(sendEnqueueFunction)) {}

void StcRpc::Test() const
{
    Message message(0, STC_TEST, nullptr);
    SendEnqueue(0, std::move(message));
}