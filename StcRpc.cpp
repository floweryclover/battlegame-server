//
// Created by floweryclover on 2024-01-27.
//

#include "StcRpc.h"

void StcRpc::RegisterSendEnqueueFunction(std::function<void(unsigned int, Message &&)> sendEnqueue)
{
    GetInstance().SendEnqueue = std::move(sendEnqueue);
}
