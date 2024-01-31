//
// Created by floweryclover on 2024-01-27.
//

#include "StcRpc.h"

StcRpc::StcRpc(std::function<void(unsigned int, Message&&)> sendEnqueue) noexcept : mSendEnqueue(std::move(sendEnqueue))
{

}
